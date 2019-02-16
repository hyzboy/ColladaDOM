<?php

/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

if(2===$COLLADA_DOM)
{
	$synth = ',';
	if(!empty($meta['isSynth']))
	{
		global $synthetics;
		$a = reset($synthetics[$meta['element_name']]);
		$synth = ', //SYNTHESIZED';
	}
	else $a = array($meta);	
	foreach($a as $ea)
	{
		$elementType = $ea['context'];
		foreach($elementType as& $ea) 
		$ea = strtoupper(asFriendlyType($ea));
		$elementType = implode($elementType,'_');
		$elementTypes[] = $elementType.'='.($genus+1).$synth;
	}
	if(count($a)>1) $elementType = $genus+1;
	else $elementType = $namespace.'_TYPE::'.$elementType;
}

if(NULL===$meta['parent_meta'])
{
	if(2!==$COLLADA_DOM)
	{
		$classname = getFriendlyName($meta['element_name']);
		
		if(!empty($recursive[$classname]))
		{
			$typedef = $classname;
			$classname = $typedef.'__recursive';
		}		
	}
	else $classname = getFriendlyType($meta['element_name']);
}
else $classname = getIntelliSenseName($meta);

//NEW: The notes must come above the class definitions
//so that the DAEP::InnerValue conversion operator can
//figure out the const-ness of the return-type.
//It would be interesting if the return-type could not
//be instantiated in order to let the translation-unit
//override DAEP::Concern (assuming One Definition Rule
//is not a factor) but this seems like it's impossible. 
//Another way was to force would-be const return-types
//to be wrapped in something like std::as_const. C++17
//To facilitate this a crazy ---> operator was thought
//about; where -- simply does the const-cast; but that
//seemed like too much baggage to take on to play with
//experiments that violate the C++ One Definition Rule.
$lo = layoutTOC($meta); 
$count_of_attributes = count($meta['attributes']);
$value = !empty($meta['content_type']);
//1+ for anyAttribute
$extent_of_values = 1+$count_of_attributes+$value;
reset($lo); $k0 = key($lo);
$_No = $extent_of_values+max($k0-1,0);
echoNotesCPP($meta,$notes,$_No);

//These come from DAEP.h.
$schema = 0; //empty-content-model
$any = $meta['flags']&ElementMeta::hasAny;
if($value) $schema = 1; //simple
if(!empty($meta['elements'])||$any) $schema = 2; //complex
if($meta['mixed']) $schema = 3; //mixed
$cm = $Content[$schema];
if($meta['parent_meta']!=NULL) $schema|=4; //inline
$schema|=$meta['flags'];
$baseclass = ($COLLADA_DOM==2?'dae':'DAEP::').'Elemental';
$schema = sprintf('0x%04x%04x%08xULL',$genus++,$count_of_attributes,$schema);
//DOCUMENTATION
echoClassDoxygentation($indent,$meta); 
echoCode("
class $1
: 
public $baseclass<$2,$schema>
{",$classname,isset($typedef)?$typedef:$classname);

if(2===$COLLADA_DOM)
{
	echoCode("
public: //COLLADA-DOM 2
	/**
	 * These were deprecated; but are kept for switch-cases.
	 */
	enum{ elementType=$elementType };
	");	
}

//INTERNAL CLASSES
//
// Pre-2.5 these were nested inside the class definitions
// But for large classes Microsoft's IntelliSense product
// would silently die for the including translation units 
//
if(!empty($meta['classes']))
{
	echo $indent,
"public: //NESTED ELEMENTS
	
";	$local__ = 2===$COLLADA_DOM?'':'local__'; 
	foreach($meta['classes'] as $k=>$ea)	
	{
		echo 
'	typedef class ', getIntelliSenseName($ea), "\n";		
		echoClassDoxygentation($indent="\t",$ea); $indent = '';
		$k = getFriendlyType($local__.$k);
		$code = "\n\t$k; ";
		//NEW: $code puts these on one line, so the comment is attached to them all
		if(2===$COLLADA_DOM)
		$code.="typedef daeSmartRef<$1> $1Ref; typedef dae_Array<$1> $1_Array; typedef daeSmartRef<const $1> const_$1Ref;";
		echoCode($code,$k);		
		echo "\n";
	}	
}
 
//The first value is expected to be anyAttribute.
echoCode("
public: //Parameters

	typedef struct:Elemental
	{   COLLADA_DOM_0");
echo $indent,
"	_0; ";
$i = 1;
foreach($meta['attributes'] as $ea)
{
	$at = getGlobalType($meta,$ea['type']);
echo	
"DAEP::Value<sizeof($i),$at>\n",$indent,
"	_$i; "; $i++;
}
if($value)
{
	$value = getGlobalType($meta,$meta['content_type']);
echo
//"DAEP::Value<$i,$value>\n",$indent,
//"	_$i; "; $i++;
"DAEP::Value<sizeof($i),$value> value;\n",$indent,
"		"; $i++;
}

//TODO: EVENTUALLY THIS SHOULD BE REPLACED
//BY SOME ATTRIBUTE DATA-MEMBERS THAT WILL 
//GET THE ALIGNMENT ON TRACK. IF THERE ARE
//NO ATTRIBUTES, ALIGNMENT ISN'T NECESSARY
//aligning contents-arrays on 64bit builds	
//echo "COLLADA_ALIGN(sizeof(_*))\n",$indent,
echo "COLLADA_WORD_ALIGN\n",$indent,
"		";
 
foreach($lo as $k=>$ea)
{
	if($k<=1) break;
echo
"DAEP::Child<sizeof($k)> //$ea->type\n",$indent, 
"	_$i; "; $i++;
}
$i-=$extent_of_values;
$count_lo = count($lo);
$singles = $count_lo-$i;

//THIS IS VERY HARD TO EXPLAIN! 
//alignN is optional-unless there are "plural"
//children involved. (they must factor alignN into
//their IDs regardless.) 0 means add the alignment
//see COLLADA_DOM_Z in the C++ sources for details
$wordsN = ceil($singles/32); $alignN = $i+$wordsN&1;
$N = $i?'1,'.($wordsN+($alignN^1)):"$alignN,$wordsN";
echo "COLLADA_DOM_Z($N)\n";

$i = $_No+($singles==0?0:1/*0 ID*/)+$singles+1; 
$contentID = $i; //save for later
$elementPtoMs = $extent_of_values;
//the generator here is 32/64bit agnostic
//dae_Array<> has pointer alignment logic 
//which COLLADA_DOM_PRECURSOR understands
echoCode("
	DAEP::Value<sizeof($_No),dae_Array<$1_Z; enum{ _No=$_No };
	DAEP::Value<sizeof($i),daeContents> content; typedef __NS__<$notes> notestart;
	}_;
",$_globals['>> ']); //support C++98/03 double-angle-bracket rules
$notes+=$i+1;

//ATTRIBUTES
if(2!==$COLLADA_DOM) 
$nc = $meta['elements']; else $nc = array();
echoCode("
public: //Attributes
");
	//TODO: will need a union{} here to implement
	//attributeGroup or to prioritize low likelihood 
	//attributes where there are large/sparse attributes
	if($schema&ElementMeta::hasAnyAttribute)
	echoAnyEtcCPP('Attribute'); else echoNoNameCPP('Attribute');
if(!empty($meta['attributes']))
{
	if($_attr=2===$COLLADA_DOM?'attr':'')
	$nc2 = array(); else $nc2 = $meta['attributes'];
	
	$i = 1;		
	foreach($meta['attributes'] as $k=>$ea)
	{
		$type = $ea['type'];
		$preType = getGlobalType($meta,$type); 	
		$type = getFriendlyType($type);
		echoDoxygen(@$ea['documentation'],"\t",$type);
		$clash = getNameClash($nc,$k,'__ATTRIBUTE');
		$name = $_attr.getFriendlyName($k.$clash);		
		
		//TODO: Children require "class" to be added sometimes.
		//I think values probably do also. See echoElementsCPP.
		
		echoCode("
	DAEP::Value<$i,$preType,_,(_::_)&_::_$i> $name;"); 
		$i++;
				
		//OVERLOAD MACROS
		if(empty($clash)) continue;	
		$name = substr($name,0,-11);
		$clash = $_attr.getFriendlyName($k);
		if($name!==$clash)
		$clash = ','.substr($clash,strlen($name));
		else $clash = '';
		echoDoxygen(@$ea['documentation'],"\t",$type,"\t/**OVERLOADED");		
		echoCode("
	COLLADA_DOM_ATTRIBUTE($1,$preType,$name$clash)",$i-1);
	}
}
else $nc2 = array();
	echo "\n";

if($el_less=$count_lo==0&&!$any)
echoCode("
public: //$cm Content Model
");
//CONTENT (VALUE)
if(!empty($value))
{
	$clash = getNameClash($nc,'value','__CONTENT',$nc2);
	echoCode("
	/**
	 * The $value value of the text data of this element. 
	 */
	DAEP::Value<$1,$value,_,(_::_)&_::value> value;"
	,$extent_of_values-1);
	
	//OVERLOAD MACROS
	if(!empty($clash))
	echoCode("
	/**OVERLOADED
	 * The $value value of the text data of this element. 
	 */
	COLLADA_DOM_CONTENT($1,$value,value)"
	,$extent_of_values-1);
}
//ELEMENTS
//Reminder: all kinds hold comments and/or processing-instructions.
//And the children are laid out ahead of the contents-array.
//TECHNICALLY THIS IS CORRECT, HOWEVER THERE SHOULDN'T BE ELEMENTS
//WHEN THERE IS A VALUE, AND SO THE //Content SECTION SHOULD MERGE.
//Reminder:
//NOW THIS OUTPUTS COLLADA_DOM_Z(X) union{ int:0; }; //USER-CHILDS
echoElementsCPP($meta,$elementPtoMs,$N,$any);
//CONTENT
if(!$el_less) 
echoCode("
public: //$cm Content Model
");
$clash = getNameClash($nc,'content','__CONTENT',$nc2);
echoCode("
	/**
	 * Children, mixed-text, comments & processing-instructions.
	 */
	DAEP::Value<$1,daeContents,_,(_::_)&_::content> content$clash;"
,$contentID);

//OVERLOAD MACROS
if(!empty($clash))
echoCode("
	/**OVERLOADED
	 * Children, mixed-text, comments & processing-instructions.
	 */
	COLLADA_DOM_CONTENT($1,daeContents,content)"
,$contentID);
	
//get/setX methods
if($COLLADA_DOM==2)
{
	echo "\n";
	echoAccessorsAndMutatorsCPP($meta);
}

echo $indent,
'};';

//INTERNAL CLASSES
//
// Pre-2.5 these were nested inside the class definitions
// But for large classes Microsoft's IntelliSense product
// would silently die for the including translation units 
//
//as&/unset($ea) is supporting echoNotesCPP and layoutTOC
foreach($meta['classes'] as& $ea) 
if(empty($ea['isSynth']))
echo "\n",applyTemplate('class-h-def',$ea); unset($ea);	

?>