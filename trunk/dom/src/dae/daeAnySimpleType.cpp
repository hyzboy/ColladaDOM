/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */
#include <ColladaDOM.inl> //PCH

COLLADA_(namespace)
{//-.
//<-'

//daeStringRef.cpp must initialize these since 
//translation-unit initialization order is not
//defined/controllable.
extern daeAnySimpleType
//NOTE: _type() is not scanning strings to see
//if they look like a (processed) xs:token, so
//a string is always daeBinaryType::STRING.
daeAnySimpleType_string,
daeAnySimpleType_hexBinary,
//NOTE: This is only met for "true" or "false"
//values, since 0 and 1 are treated as numbers.
daeAnySimpleType_boolean,
#ifdef NDEBUG
//A storm has cutoff my Internet as I type this
#error Can daeTypist<float> throw an error on double precision values?
#endif
//TODO? Might want this for arrays but I worry
//isteam's extraction operator will gladly lop
//off double precision figures.
//daeAnySimpleType_float,
daeAnySimpleType_double, 
//NOTE: Assigning all numbers without a - sign
//to unsigned types just seems like a good way
//to avoid issues. It is a problem for blindly
//assigned binary values. 
//NOTE: daeAnySimpleTypewriter::compare thinks
//(signed) xs:long<xs:int and xs:uint<xs:ulong.
daeAnySimpleType_int,daeAnySimpleType_uint,
daeAnySimpleType_long,daeAnySimpleType_ulong;



	//// daeMetaAttribute.h //// daeMetaAttribute.h //// daeMetaAttribute.h ////
	//// daeMetaAttribute.h //// daeMetaAttribute.h //// daeMetaAttribute.h ////
	//// daeMetaAttribute.h //// daeMetaAttribute.h //// daeMetaAttribute.h ////

 

daeAST::TypedUnion::~TypedUnion()
{	
	switch(_type->getAtomicType())
	{	
	case daeAtomicType::EXTENSION: //array?

		switch(_type->where<daeAtom>().getAtomicType())
		{
		//case daeAtomicType::STRING:
		//	((daeArray<daeStringRef>*)&_union.list)->clear();
		//	break;
		case daeAtomicType::BINARY:
			((daeArray<daeBinary<16>>*)&_union.list)->clear();
			break;
		}
		getObject()->_deAlloc(_union.list);
		break;

	case daeAtomicType::BINARY:

		((daeBinary<16>*)&_union.hexBinary)->~daeBinary();
		break;

	case daeAtomicType::STRING: 
	
		if('\0'!=_union.string[0])
		((daeStringRef*)&_union.string)->~daeStringRef();
		break;
	}		
}
daeAST::TypedUnion::TypedUnion():_offset()
{
	_type = daeAnySimpleType_string;
	_union.string = daeStringRef_empty; 
}
daeAST::Data::Data(daeElement *e, daeString DOM_empty)
{
	//_type = daeAnySimpleType_string;
	_union.string = DOM_empty;
	//Putting these into the system pool in order to
	//have better control. This avoids calling their
	//destructor (~Data()) and leaves open an option
	//to use &1 instead of isStatic to not store the
	//flags (all 0) in Data attributes.
	//_attribute_name_string = DOM_empty;
	_attribute_name_string = daeStringRef_empty;

	#ifdef _DEBUG
	__vizDefault = nullptr;
	#endif

	#ifdef NDEBUG
	#error Maybe have daeTypewriter set this for nonempty strings 
	#endif
	//WARNING: This is not a bad compromise since these should be rare.
	//Except for domAny. However, domAny is by its very nature unideal.
	_this_attribute_is_ID = 1;

	daeOffset hack1 = (daeOffset)(&_attribute_name_string+1);
	daeOffset hack2 = (daeOffset)(&_offset);
	memset((void*)hack1,0x00,hack2-hack1); //(bitfields)
	_offset = daeOffset(&_union)-daeOffset(e);
	//See this lines up with daeData.
	assert(_type==&getDefaultType());
}	 

void daeAST::TypedUnion::_copy(const TypedUnion &cp)
{
	#ifdef NDEBUG
	#error Pretty sure _copy is untested!
	#endif
	daeAlloc<> *au;	   
	switch(cp._type->getAtomicType())
	{	
	default: //POD type?
	
		this->~TypedUnion(); _type = cp._type; //_reset_type
		#ifdef _DEBUG
		//Can help with debugger readability... maybe more so
		//in little-endian modes.
		memset(&_union,0x00,sizeof(_union));
		memcpy(&_union,&cp._union,_type->getSize());		
		#else		
		memcpy(&_union,&cp._union,sizeof(double));
		#endif
		assert(_type->getSize()<=sizeof(double));
		break;

	case daeAtomicType::BINARY:

		au = cp._union.hexBinary.AU->unit();
		goto au;

	case daeAtomicType::EXTENSION: //array?
	{
		assert(cp._type->hasListType()); 
		au = cp._union.list;

	au:	size_t size = au->getInternalCounter();
	
		if(_type!=cp._type)
		cp._type->_reset_type(&_union,size);
		daeAlloc<> **au2 = &_union.list;

		daeTypewriter &at = _type->where<daeAtom>();
		switch(at.getAtomicType())
		{
		//Need to handle non POD types... except
		//string arrays shouldn't be possible since
		//spaces are interpreted as part of the string.
		//case daeAtomicType::STRING:
		case daeAtomicType::BINARY:

			if(au==cp._union.list)
			{
				*(daeArray<daeBinary<16>>*)&_union.list = 
				*(daeArray<daeBinary<16>>*)&cp._union.list;
				break;
			}
			au2 = (daeAlloc<>**)&_union.hexBinary.AU;
			//break;

		default: //May as well raw-copy the POD data?

			if((*au2)->getCapacity()<size)			
			getObject()->reAlloc(*au2,size);
			int va = &au->getRaw()-(char*)au;
			memcpy((char*)*au2+va,(char*)au+va,at.getSize()*size);
			(*au2)->setInternalCounter(size);
		}
		break;
	}
	case daeAtomicType::STRING: 
	
		if(_type!=daeAnySimpleType_string)
		daeAnySimpleType_string->_reset_type(&_union,0);
		*(daeStringRef*)&_union.string = 
		*(daeStringRef*)&cp._union.string;
		break;
	}	
}
void daeAST::TypedUnion::_prototype_construct()
{
	//HACK: Trying leverage _copy implementation.
	void *hack[sizeof(TypedUnion)/sizeof(void*)];
	memcpy(hack,this,sizeof(*this));
	_type = daeAnySimpleType_int; //Any POD here.
	_copy(*(TypedUnion*)hack);
}



	//// daeAtomicType.h //// daeAtomicType.h //// daeAtomicType.h ////
	//// daeAtomicType.h //// daeAtomicType.h //// daeAtomicType.h ////
	//// daeAtomicType.h //// daeAtomicType.h //// daeAtomicType.h ////



size_t daeAnySimpleTypewriter::_size(daeOpaque cp)
{
	switch(_atomType)
	{
	case daeAtomicType::EXTENSION: //assuming array
	{
		daeAlloc<>* &p = cp; return p->getInternalCounter();
	}
	case daeAtomicType::BINARY:
	{
		daeBinary<16>* &p = cp; return p->size();
	}}
	return 0;
}
bool daeAnySimpleTypewriter::_reset_type(daeOpaque io, size_t sz)
{
	//d is not necessary a Data, but it is equivalent here.
	daeAST::Data &d = io[-daeOffsetOf(daeAST::Data,_union)];
	d.TypedUnion::~TypedUnion();

	#ifdef _DEBUG
	//Can help with debugger readability... maybe more so
	//in little-endian modes.
	memset(&d._union,0x00,sizeof(d._union));
	#endif
	
	//Need to set _type before BINARY resets io. Note the
	//type is set by _failure_to_unserialize if returning
	//"false."
	_type(io) = this;

	//TODO? Opposite of ~Union().	
	daeOffset os = d.getOffset();
	switch(_atomType)
	{
	case daeAtomicType::BINARY: 
		
		goto au;

	case daeAtomicType::EXTENSION: //array?
	{
		assert(hasListType());

	au:	daeTypewriter &at = where<daeAtom>();
		size_t unitsz = at._sizeof;

		daeAlloc<char,0> thunk;				
		const daeAlloc<> *vptr = 0;
		switch(at._atomType)
		{
		case +int(sizeof(int)):
		case -int(sizeof(int)):
		vptr = &daeAlloc<int>::localThunk();
		break;
		case +int(sizeof(long long)):
		case -int(sizeof(long long)):
		vptr = &daeAlloc<long long>::localThunk();
		break;
		case COLLADA_DOM_ATOMIC_FLOAT(double):
		vptr = &daeAlloc<double>::localThunk();
		break;
		case daeAtomicType::BINARY:
		if(!hasListType())
		{
			unitsz = sizeof(char);				
			vptr = &daeAlloc<char>::localThunk();
			new(&io) daeBinary<16>(*thunk.unit());
			daeOffset header = 
			daeOffsetOf(daeAST::Data::Union::Binary,AU);
			os+=header; 
			io = io[header];
		}
		else vptr = &daeAlloc<daeBinary<16>>::localThunk();
		break;
		case daeAtomicType::BOOLEAN:
		vptr = &daeAlloc<daeBoolean>::localThunk();
		break;
		case daeAtomicType::STRING:		
		vptr = &daeAlloc<daeStringRef>::localThunk();
		break;
		//PROGRAMMER ERROR
		default: assert(0);
		case daeAtomicType::EXTENSION:		
		assert(0);
		//if(unitsz!=sizeof(d._union))		
		return false; 
		//This type is not implemented.
		//vptr = &daeAlloc<daeAST::TypedUnion>::localThunk();
		//break;
		}

		//Reminder: BINARY adjusts io, os, unitsz!
		daeArray<char> &a = io;
		thunk->_offset = os;
		a.getAU() = thunk.unit();
		a.grow(unitsz*sz);		
		
		daeAlloc<char> *au = a.getAU();
		vptr->moveThunk(au,au->getCapacity()/unitsz);
		(*au)->_offset = os;
		break;
	}	
	case daeAtomicType::STRING:
	{
		daeString &r = io; 
		if(os==0) r = daeStringRef_empty;
		else r = d.getElement().getDOM()->getEmptyURI().data();
		break;
	}}

	return true;
}

daeAnySimpleTypewriter*
daeAnySimpleTypewriter::_type(daeString srcIn, daeString srcEnd, size_t &size)
{	
	enum
	{	
	SIGN=1, //NOTE: INF wants SIGN==1!
	DP=2,DEC=4,E=8,ESIGN=16,NaN_OR_INF=32,
	FSIG=SIGN|DP|DEC|E|ESIGN|NaN_OR_INF, 
	HEX=64,true_OR_false=128,
	};

	daeAnySimpleTypewriter *out = 0;

	int spaces = 0;
	int sflags = 0; 
	int mflags = 0;
	//daeString p,ws=srcEnd;
	for(daeString p=srcIn;p<srcEnd;p++) switch(*p)
	{				
	//THIS IS xs:string with surrounding whitespace
	//left intact. It is this way to not have to do
	//a full scan and because it's mostly ambiguous.
	default: goto string;

	case '-': case '+': 

		if(sflags==0)
		{
			sflags|=SIGN; break;
		}
		//scientific notation?
		if(0!=(sflags&E)) //E?			
		if(0==(sflags&~(SIGN|DP|DEC|E)))
		{
			sflags|=ESIGN; break;
		}
		goto string;	
		
	case ' ': 
	case '\t': case '\r': case '\n': 

		if(sflags!=0)
		{
		//	ws = p;						
			mflags|=sflags;
			sflags = 0;
		}
		//else if(p-ws==1) 
		{		
			//short-circuiting strings 
			//non_token_string = true;
		}
		spaces++; break;

	case 'I': //INF?

		if(sflags<=SIGN) //1
		if(p[1]=='N'&&p[2]=='F')
		{
			NaN_or_INF:
			if('\0'==p[3]||COLLADA_isspace(p[3]))
			{
				sflags|=NaN_OR_INF; p+=2; break;
			}
		}
		goto string; 	

	case 'N': //NaN?

		if(sflags==0)
		if(p[1]=='a'&&p[2]=='N')
		goto NaN_or_INF;
		goto string;
	
	case '.': //decimal point?

		sflags|=DP; break;

	case 'E': case 'e': //scientific notation?

		if(0==(sflags&~(SIGN|DP|DEC)))
		{
			if(0!=(sflags&(DP|DEC)))
			{
				sflags|=E; break;
			}
		}
		goto e;

	case 'f':

		if(p[2]=='l'&&p[1]=='a')
		if(p[3]=='s'&&p[4]=='e')
		if('\0'==p[5]||COLLADA_isspace(p[5]))
		{
			sflags|=true_OR_false; p+=4; break;
		}
		goto f;

	case 'a': case 'b': case 'c': case 'd': e: f:
	case 'A': case 'B': case 'C': case 'D': case 'F': 

		sflags|=HEX; break;
		
	case '1': case '2': case '3': case '4': case '5': 
	case '6': case '7': case '8': case '9': case '0': 

		sflags|=DEC; break;		

	case 't':

		if(p[1]=='r'&&p[2]=='u')
		if(p[3]=='e')
		if('\0'==p[4]||COLLADA_isspace(p[4]))
		{
			sflags|=true_OR_false; p+=3; break;
		}
		goto string;
	}
	mflags|=sflags;

	if(spaces!=0)
	{
		daeOffset diff = srcEnd-srcIn;
		daeTypist<>::normalize(srcIn,srcEnd);
		spaces-=diff-(srcEnd-srcIn); 
		size = spaces+1;		
	}
		
	switch(mflags)
	{
	case true_OR_false:

		//NOTE: Excludes 0 or 1 values.
		out = daeAnySimpleType_boolean;
		break;

	case DEC:

		//Go to HEX|DEC if longer than 64bit.
		if(srcEnd-srcIn>20&&spaces==0) 
		goto hexBinary;

		//WARNING: ulong/uint is relying on
		//_failure_to_unserialize to promote
		//integers/arrays to 64-bit values...
		//ASSUMING 64BIT IS A RARE OCCURRENCE.
		out = daeAnySimpleType_uint;
		break;
	case SIGN|DEC: 
		out = daeAnySimpleType_int;
		break;

	case HEX|DEC: //xs:hexBinary 

		//Treat as a string if it there's any
		//possibility it is a word/identifier.		
		if(srcEnd-srcIn<32)	
		goto string;
	
	hexBinary:
				
		//Try to avoid odd number hexadecimal
		//error/false-positive? Without going
		//to great lengths.
		if((srcEnd-srcIn-spaces)%2==1)
		goto string;

		//size is the size of the hexBinary 
		//array, unless there are spaces, in
		//which case it's understood to be an
		//xs:list and size refers to the items. 
		if(spaces==0)
		{
			size = (srcEnd-srcIn)/2+1;
		}
		out = daeAnySimpleType_hexBinary;		
		break;

	default: //float?

		if(0!=(mflags&~FSIG))
		{	
			assert(0!=spaces);
			//Mixed type array? (UNIMPLEMENTED)
			//return daeAnySimpleType_anySimpleType+1;
		}
		else //float?
		{
			//REMINDER: _failure_to_unserialize
			//will catch mistakes.
			assert(0!=(mflags&FSIG));

			//NOTE: would try for float for an
			//array, since it's less memory, but
			//worry that istream::operator>> won't
			//error provided double precision values.
			out = daeAnySimpleType_double;
			break;
		}
		goto string;

	case DP: //A period? 
	case DP|HEX: //E.g. "cad.dae."
	case SIGN: //A dash? 
	case 0: //empty/whitespace strings?	
	string: //last resort

		//REMINDER: Cannot be an array.
		return daeAnySimpleType_string;
	}
	return out+(0==spaces?0:1);
}
daeOK daeAnySimpleTypewriter::_failure_to_unserialize(daeString srcIn, daeString srcEnd, daeOpaque io, size_t size)
{
	daeAnySimpleTypewriter *promote;
	switch(where<daeAtom>().getAtomicType())
	{
	//_type(srcIn,srcEnd) doesn't attempt to recognize arrays 
	//with xs:long width integers. So just in case try 64-bit.
	//NOTE: This is more likely to be so for arrays but could
	//happen also if the number of digits could go either way.
	case +int(sizeof(int)): 		
		promote = daeAnySimpleType_ulong+(hasAtomicType()?0:1);
		break;
	case -int(sizeof(int)): 			
		promote = daeAnySimpleType_long+(hasAtomicType()?0:1);
		break;

	default: string: //last resort

		COLLADA_SUPPRESS_C(4551)
		assert(!"_failure_to_unserialize");
		daeEH::Error<<"daeAnySimpleTypewriter::_failure_to_unserialize()\n"
		<<"PROGRAMMER ERROR: Unexpectedly reverting non-schema/xs:anySimpleType data to xs:string...\n"
		<<daeHashString(srcIn,std::min<size_t>(srcEnd-srcIn,64));
		daeAnySimpleType_string->_reset_type(io,0);
		return daeAnySimpleType_string->_daeAtomWriter->unserialize(srcIn,srcEnd,io);
	}
	promote->_reset_type(io,size);
	if(!promote->_typewriter()->unserialize(srcIn,srcEnd,io))
	goto string; return DAE_OK;
}

//---.
}//<-'

/*C1071*/
