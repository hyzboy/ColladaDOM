/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */
#include <ColladaDOM.inl> //PCH
#include <ColladaDOM.g++> //GCH

COLLADA_(namespace)
{//-.
//<-'

	//TODO: It seems like daeAnySimpleType_xmlns should have its own dedicated
	//daeAtomicType::XMLNS in order to avoid the string table confusion and to
	//not have to lookup the string in the first place.

	
//daeStringRef.cpp must initialize these since 
//translation-unit initialization order is not
//defined/controllable.
extern daeAST::AnyWriter
daeAnySimpleType_void,
daeAnySimpleType_xml, //HACK: xml::base/xmlns
daeAnySimpleType_string;
extern daeAnySimpleType
//NOTE: _type() is not scanning strings to see
//if they look like a (processed) xs:token, so
//a string is always daeBinaryType::STRING.
daeAnySimpleType_hexBinary,
//NOTE: This is only met for "true" or "false"
//values, since 0 and 1 are treated as numbers.
daeAnySimpleType_boolean,
#ifdef NDEBUG
#error Can daeTypist<float> throw an error on double precision values?
#endif
//TODO? Might want this for arrays but I worry
//isteam's extraction operator will gladly lop
//off double precision figures.
//daeAnySimpleType_float,
daeAnySimpleType_double, 
daeAnySimpleType_pint, //daeAST::Positive 
daeAnySimpleType_int,daeAnySimpleType_uint,
daeAnySimpleType_plong, //daeAST::Positive
daeAnySimpleType_long,daeAnySimpleType_ulong;

//setPOD_representation implementation.
COLLADA_(extern) daeAnySimpleTypewriter *daeAnySimpleType_reps[] = 
{
	daeAnySimpleType_int, //daeAnySimpleType_pint
	daeAnySimpleType_uint,	
	daeAnySimpleType_long, //daeAnySimpleType_plong
	daeAnySimpleType_ulong,
	daeAnySimpleType_double,
	daeAnySimpleType_boolean,
	&daeAnySimpleType_void, //May be changing VOID to 0/POD type.
	&daeAnySimpleType_string, //setPOD_representation terminator.
	daeAnySimpleType_hexBinary
};

//Implementing TypedUnion::is with int/long long.
static daeAnySimpleType &daeAnySimpleType_(int i)
{
	return i<0?daeAnySimpleType_int:daeAnySimpleType_uint; 
}
static daeAnySimpleType &daeAnySimpleType_(long long i)
{
	return i<0?daeAnySimpleType_long:daeAnySimpleType_ulong; 
}
template<class I, class J>
inline daeOK daeAST::Positive<I,J>::
AtomWriter::unserialize(daeString srcIn, size_t srcN, daeOpaque dst)
{
	//NEW: Adding appending model/srcN.
	if(0!=srcN>>sizeof(srcN)*CHAR_BIT-1)
	return DAE_ERROR;
	daeString srcEnd = srcIn+srcN;

	this->normalize(srcIn,srcEnd);
	daeIStringBuf buf(srcIn,srcEnd);
	std::istream src(&buf);	
	//daeOK OK = Typist::encodeXML(src,*(I*)&dst);
	//if(OK==DAE_OK&&(!src.eof()||src.fail())) OK = DAE_ERROR; return OK;
	{
		src >> *(I*)&dst;
				
		if(src.fail())
		{
			src.clear();

			src >> *(J*)&dst;

			if(src.fail()||!src.eof()) return DAE_ERROR;

			AnyWriter::_type(dst) = daeAnySimpleType_(I()); 
		}
		else if(!src.eof()) return DAE_ERROR; return DAE_OK;
	}
}
template<class I, class J>
inline daeOK daeAST::Positive<I,J>::
ArrayWriter::unserialize(daeString srcIn, size_t srcN, daeOpaque dstIn)
{
	daeArray<I> &dst = dstIn;

	//NEW: Promise not appended in err.
	size_t rollback;

	//NEW: Adding appending model/srcN.
	if(0!=srcN>>sizeof(srcN)*CHAR_BIT-1)
	{
		srcN = ~srcN;			
		if(srcN!=0){ rollback = dst.size(); goto append; }
	}
	else dst.clear(); rollback = 0; append:
	daeString srcEnd = srcIn+srcN;
		
	this->normalize(srcIn,srcEnd);
	daeIStringBuf buf(srcIn,srcEnd); 
	std::istream src(&buf);	
	for(I tmp;!src.eof();)
	{
		//OK = Typist::encodeXML(src,tmp);
		//if(OK!=DAE_OK) break;
		{
			src >> tmp;

			if(src.fail())
			{
				src.clear();

				src >> *(J*)&tmp;

				if(src.fail()) return DAE_ERROR;

				//Detour: Is the array positive?
				for(size_t i=0;i<dst.size();i++) if(dst[i]<0)
				{
					//This has to be done in case there are
					//earlier negative values in this assignment
					//or if client code had set values with get<int>().
					AnyWriter::_type(dstIn) = daeAnySimpleType_(I(-1))+1;
					return DAE_ERROR;										
				}
				
				//Switching to unsigned loop.
				AnyWriter::_type(dstIn) = daeAnySimpleType_(I())+1;
				goto jump_in; for(;!src.eof();)
				{
					src >> *(J*)&tmp;

					if(src.fail()) return DAE_ERROR;

				jump_in: dst.push_back(tmp);
				}
				return DAE_OK;
			}
		}		
		dst.push_back(tmp);
	}
	return DAE_OK;
}
COLLADA_(extern) daeAST::PositiveI daeAnySimpleType_i;
COLLADA_(extern) daeAST::PositiveLL daeAnySimpleType_ll;

extern const daeAllocThunk daeElement_thunk0;

daeAST::TypedUnion::~TypedUnion()
{
	daeAlloc<char>* &au = _TypedUnion();
	COLLADA_ASSUME(!au->isEmbeddedAU(au))
	getObject().deAlloc(au);
}
daeAlloc<char>* &daeAST::TypedUnion::_TypedUnion()
{	
	daeAlloc<> **au; if(0==_type->_list)
	{
		if(is<daeString>()
		&&'\0'!=_union.string[0])
		((daeStringRef*)&_union)->~daeStringRef();		
		au = &_union.hexBinary.AU;
	}
	else
	{
		au = &_union.list;	
		if(0!=(*au)->getInternalCounter()
		&&!_type->_atomWriter->isPOD())
		_type->clear(&_union);		
	}
	(*au)->setInternalCounter(0); return *(daeAlloc<char>**)au;
}
daeAST::TypedUnion::TypedUnion():_offset()
{
	//TODO: Expose these by externally visible global.
	_type = &daeAnySimpleType_void;
	_union._long = 0;
	(const void*&)_union.hexBinary.AU = &daeElement_thunk0;
}
daeAST::Data::Data(daeElement *e)
{
	//NOTE: If the DOM's empty string is to be
	//used here daeAnyAttribute::_grow should be
	//passing it to this contructor in bulk. Right
	//now the names are going into the system's pool.
	//If necessary isValue can be changed to check the
	//first bit of the pointer to determine if names are
	//in the system pool, assuming client-strings are even.
	_attribute_name_string = daeStringRef_empty;

	#ifdef _DEBUG
	__vizDefault = nullptr;
	#endif

	daeOffset hack1 = (daeOffset)(&_attribute_name_string+1);
	daeOffset hack2 = (daeOffset)(&_offset);
	memset((void*)hack1,0x00,hack2-hack1); //(bitfields)
	_anySimpleType = 1;		
	/*Setting in _reset_type	
	//WARNING: This is not a bad compromise since these should be rare.
	//Except for domAny. However, domAny is by its very nature unideal.
	_this_attribute_is_ID = 1;
	*/
	_offset = daeOffset(&_union)-daeOffset(e);
	//See this lines up with daeData.
	assert(_type==&getDefaultType());
}	 

void daeAST::TypedUnion::_prototype_construct()
{
	//HACK: Try to leverage _copy implementation.
	void *hack[sizeof(TypedUnion)/sizeof(void*)];
	memcpy(hack,this,sizeof(*this));	
	(const void*&)_union.hexBinary.AU = &daeElement_thunk0;
	_type = daeAnySimpleType_int; //Any POD here.
	_copy(*(TypedUnion*)hack);
}
void daeAST::TypedUnion::_copy(const TypedUnion &cp)
{
	#ifdef NDEBUG
	#error Pretty sure _copy is untested!
	#endif
	daeAlloc<> *au;	   
	switch(int at=cp._type->getAtomicType())
	{	
	default: //POD type?
	
		//~TypedUnion(); 
		_union.hexBinary.AU = 
		(daeAlloc<>*)_TypedUnion();
		_union._long = 0;		
		_type = cp._type;

		break;

	case daeAtom::BINARY:

		au = cp._union.hexBinary.AU;
		goto au;

	case daeAtom::EXTENSION: //array?
	{
		assert(cp._type->hasListType()); 
		au = cp._union.list;

	au:	size_t size = au->getInternalCounter();
	
		if(_type!=cp._type)
		cp._type->_reset_type(&_union,size);
		daeAlloc<> **au2 = &_union.list;

		daeTypeInfo &ai = _type->where<daeAtom>();
		switch(ai._atomType)
		{
		//Need to handle non POD types... except
		//string arrays shouldn't be possible since
		//spaces are interpreted as part of the string.
		//case daeAtom::STRING:
		case daeAtom::BINARY:

			//Removing very theoretical false-positive.
			//if(au==cp._union.list)
			if(at==daeAtom::EXTENSION)
			{	
				*(daeArray<daeBinary<16>>*)&_union = 
				*(daeArray<daeBinary<16>>*)&cp._union;
				break;
			}
			else au2 = &_union.hexBinary.AU;
			//break;

		default: //May as well raw-copy the POD data?

			if((*au2)->getCapacity()<size)			
			getObject().reAlloc(*au2,size);
			int va = &au->getRaw()-(char*)au;
			memcpy((char*)*au2+va,(char*)au+va,ai.getSize()*size);
			(*au2)->setInternalCounter(size);
		}
		break;
	}
	case daeAtom::STRING: 
	
		if(_type!=&daeAnySimpleType_string)
		daeAnySimpleType_string._reset_type(&_union,0);
		*(daeStringRef*)&_union = *(daeStringRef*)&cp._union;
		break;
	}	
}



	//// daeAtomicType.h //// daeAtomicType.h //// daeAtomicType.h ////
	//// daeAtomicType.h //// daeAtomicType.h //// daeAtomicType.h ////
	//// daeAtomicType.h //// daeAtomicType.h //// daeAtomicType.h ////



size_t daeAST::AnyWriter::_size(daeOpaque cp)
{
	switch(_atomType)
	{
	case daeAtom::EXTENSION: //assuming array
	{
		daeAlloc<>* &p = cp; return p->getInternalCounter();
	}
	case daeAtom::BINARY:
	{
		daeBinary<16>* &p = cp; return p->size();
	}
	case daeAtom::VOID: return 0; default: return 1;
	}
}
bool daeAST::AnyWriter::_reset_type(daeOpaque io, size_t sz)
{	 
	//d is not necessary a Data, but it is equivalent here.
	daeAST::Data &d = io[-daeOffsetOf(daeAST::Data,_union)];
	daeOffset os = d.getOffset();

	daeAST::AnyWriter* &type = _type(io);

	//NOTE: setPOD_representation CAN DEFEAT THIS.
	//HACK: Inequality here includes daeAnySimpleType_void.	
	if(type<=&daeAnySimpleType_xml&&os!=0) if(0!=sz)
	{	
		#ifdef NDEBUG
		#error What about "resolveXMLNS" for attributes?
		#endif
		
		//This is code that should logically be done while
		//the attribute is being created/named, however it
		//would add really obscure overhead to header only
		//code that could bloat if inlined or keep from it
			
		daeAttribute *d2;
		const daeElement &e = d.getElement();
		//if(!d.isAnyExtra())
		if((size_t)d._offset<e.getMeta().getSize())
		{
			//YUCK: Filtering out non daeAST::Data data is
			//necessary, however the code below is all for
			//letting xml:base go through. It has no other
			//way than to catch it than to locate its name.

			//FYI: This is an early out for domAny
			//but is also necessary to use front().
			//if(!e.getMeta().getAttributes().empty())			
			//d2 =e.getMeta().getAttributes().front();
			//else goto xml;			
			if(0==e.getMeta()._systemAttribute)
			goto xml;
			//FYI: daeMeta::getValue should stop this loop.
			//Although _systemAttribute now excludes value.
			d2 = e.getMeta().getAttributes().front();			
			while(d2->getOffset()<os) 
			d2++; 
			assert(d2->getOffset()==os);			
			if('x'!=*d2->getName()) goto xml;
		}
		else //Normal operating procedure:
		{
			d2 = (daeAttribute*)&d;					
			//HACK/OPTIMIZING: Assuming ID-like if stringy.
			//Assuming "true" and "false" are unlikely IDs.
			if(this==&daeAnySimpleType_string)
			{
				//ASSUMING DESIRABLE/AFTER CHANGE-NOTICE!!
				d.setIsID();
			}		
		}
		daeName q = d->getName(); switch(q.extent)
		{	
		default:switch(d->getNCNameCP())
				{
				case 4: 

					if('x'==q.string[0]
					 &&'m'==q.string[1]
					 &&'l'==q.string[2])
					{
						q.string+=4; q.extent-=4;
						int b = 8;
						if("base"==q) b = 2;
						if("lang"==q) b = 4;						
						d->_setIsXml(b,&e);
						if(b!=2) goto xml; //base?

						//Enable xml:base processing on document.
						const daeDocument *doc = e.getDocument();
						if(doc!=nullptr)
						d->_setIsXml(b,&doc->getPseudoElement());						
						
						d._union.string = e.getDOM()->getEmptyURI().data();						
						type = &daeAnySimpleType_xml;

						//Trigger _failure_to_unserialize because
						//"this"/_unserialize2 will be mismatched.
						return false;
					}
					break;
				case 6: q.extent = 5; goto C2226; //goto xmlns;
				}				
				break;
		case 5: C2226: //xmlns: //Collides with COLLADA::xmlns?
			if(q=="xmlns") 
			{	
				const_cast<XS::Attribute*>(d2)->_setIsXmlns(&e);

				//"xmlns" always enter this block since 
				//its type is determined by name and not
				//the contents of its value.
				type = &daeAnySimpleType_xml;

				//HACK: Switch to system-pool strings.
				//NOTE: Might leak refs if the string
				//data has been assigned too directly.
				//But if code does that it won't have
				//triggered _setIsXmlns.
				d._union.string = daeStringRef_empty;									
				
				//Trigger _failure_to_unserialize because
				//"this"/_unserialize2 will be mismatched.
				return false;
			}
		}		
	}	
	else switch(_atomType) //sz should be nonzero.
	{
	//Technically possible via _size() but should be unlikely.
	case daeAtom::BINARY: case daeAtom::EXTENSION: xml: break;
	default: assert(this==&daeAnySimpleType_void);
	}
	

	//d.~TypedUnion();
	daeAlloc<char> *au = d._TypedUnion();
	(void*&)d._union.hexBinary.AU = au;	
	
	//Need to set _type before BINARY resets io. Note the
	//type is set by _failure_to_unserialize if returning
	//"false."
	//_type(io) = this;
	type = this;
	
	switch(_atomType)
	{
	default: d._union._long = 0; break;

	case daeAtom::BINARY: 
		
		goto au;

	case daeAtom::EXTENSION: //array?
	{
		assert(hasListType());
							 
 	au:	daeTypewriter &at = where<daeAtom>();
		
		size_t unitsz = at.getSize();
		size_t cap = au->getCapacity();
		if(0!=cap)		
		cap = cap*au->getElementSize();
		daeAlloc<char,0> thunk(cap);
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
		case daeAtom::BINARY:
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
		case daeAtom::UBOOLEAN:
		vptr = &daeAlloc<bool>::localThunk(); //daeBoolean
		break;
		case daeAtom::STRING:		
		vptr = &daeAlloc<daeStringRef>::localThunk();
		break;
		//PROGRAMMER ERROR
		default: assert(0);
		case daeAtom::EXTENSION:		
		assert(0);
		//if(unitsz!=sizeof(d._union))	
		type = &daeAnySimpleType_void;
		return false; 
		//This type is not implemented.
		//vptr = &daeAlloc<daeAST::TypedUnion>::localThunk();
		//break;
		}

		//Reminder: BINARY adjusts io, os, unitsz!
		daeArray<char> &a = io;
		thunk->_offset = os;
		if(0!=cap)
		memcpy(au,&thunk,sizeof(thunk));
		else au = thunk.unit();			
		a.getAU() = au; 
		a.grow(unitsz*sz);		
		au = a.getAU();
		vptr->moveThunk(au,au->getCapacity()/unitsz);		

		//Quiet assert(_capacity<=1); 
		#ifdef _DEBUG
		memset(&thunk,0x00,sizeof(thunk));
		#endif
		break;
	}	
	case daeAtom::STRING:
	{
		d._union.string = 
		os==0?daeStringRef_empty:
		d.getElement().getDOM()->getEmptyURI().data();
		break;
	}}

	return true;
}

daeAST::AnyWriter*
daeAST::AnyWriter::_type(daeString srcIn, size_t srcN, size_t &size)
{	
	//NEW: Adding appending model/srcN.
	assert(0==srcN>>sizeof(srcN)*CHAR_BIT-1);
													 
	enum
	{	
	SIGN=1, //NOTE: INF wants SIGN==1!
	DP=2,DEC=4,E=8,ESIGN=16,NaN_OR_INF=32,
	FSIG=SIGN|DP|DEC|E|ESIGN|NaN_OR_INF, 
	HEX=64,true_OR_false=128,
	};

	daeAST::AnyWriter *out = 0;

	size_t spaces = 0;
	size_t sflags = 0; 
	size_t mflags = 0;
	daeString srcEnd = srcIn+srcN;
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
		daeTypist<>::normalize(srcIn,srcEnd);
		size_t swap = srcEnd-srcIn;
		spaces-=srcN-swap; 
		srcN = swap;
		size = spaces+1;		
	}
	else size = 1;
		
	switch(mflags)
	{
	case true_OR_false:

		//NOTE: Excludes 0 or 1 values.
		out = daeAnySimpleType_boolean;
		break;

	case DEC:

		//Go to HEX|DEC if longer than 64bit.
		if(srcN>20&&spaces==0) 
		goto hexBinary;

		//WARNING: ulong/uint is relying on
		//_failure_to_unserialize to promote
		//integers/arrays to 64-bit values...
		//ASSUMING 64BIT IS A RARE OCCURRENCE.
		//out = daeAnySimpleType_uint;
		//TBD: Because is<int> is insufficient
		//if there are no negative values this
		//writer acts like _int unless it runs
		//into a value larger than INT_MAX, at
		//which time it switches to <unsigned>.
		out = daeAnySimpleType_pint;
		break;
	case SIGN|DEC: 
		out = daeAnySimpleType_int;
		break;

	case HEX|DEC: //xs:hexBinary 

		//Treat as a string where there's any
		//possibility it is a word/identifier.		
		if(srcN<32)	
		goto string;
	
	hexBinary:
				
		//Try to avoid odd number hexadecimal
		//error/false-positive? Without going
		//to great lengths.
		if((srcN-spaces)%2==1)
		goto string;

		//size is the size of the hexBinary 
		//array, unless there are spaces, in
		//which case it's understood to be an
		//xs:list and size refers to the items. 
		if(spaces==0)
		{
			size = srcN/2+1;
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
		
	case 0: //Whitespace strings?	

		//Implements daeElement::remove Attriubte.
		//No, "" is interpreted as a string, as XML
		//allows attributes to be assigned to be "".
		if(srcIn==nullptr)
		{
			size = 0; return &daeAnySimpleType_void;
		}
		else string: size = 1; //Nonzero if not VOID.

	case DP: //A period? 
	case DP|HEX: //E.g. "cad.dae."
	case SIGN: //A dash? 		

		//REMINDER: Cannot be an array.
		return &daeAnySimpleType_string;
	}
	return out+(0==spaces?0:1);
}
daeError daeAST::AnyWriter::_failure_to_unserialize(daeString srcIn, size_t srcN, daeOpaque dst, size_t size)
{	
	//NEW: Adding appending model/srcN.
	if(0!=srcN>>sizeof(srcN)*CHAR_BIT-1)
	{	
		srcN = ~srcN;
		if(0==srcN) //Helpful?
		return DAE_OK;
		daeArray<daeStringCP,COLLADA_MAX_PATH> buf;
		serialize(dst,buf);		
		buf.push_back(' ');
		buf.append(srcIn,srcN); 
		return unserialize(buf.data(),buf.size(),dst).error;
	}

	daeAST::AnyWriter *promote;
	switch(where<daeAtom>().getAtomicType())
	{
	//_type(srcIn,srcEnd) doesn't attempt to recognize arrays 
	//with xs:long width integers. So just in case try 64-bit.
	//NOTE: This is more likely to be so for arrays but could
	//happen also if the number of digits could go either way.
	case +int(sizeof(int)): 		
		//promote = daeAnySimpleType_ulong+(hasAtomicType()?0:1);
		//break;
	case -int(sizeof(int)): 			
	{	//NEW: Assign plong if '-' is not present in order to
		//better support TypedUnion::is<long long>().
		//promote = daeAnySimpleType_long+(hasAtomicType()?0:1);
		int list = _list;
		if(_type(dst)==daeAnySimpleType_int+list)
		promote = daeAnySimpleType_long+list; 
		else
		promote = daeAnySimpleType_plong+list;
		break;
	}
	case daeAtom::STRING: //xmlns?
	{
		//XMLNS always triggers _failure_to_unserialize so it
		//can keep using the system-pool to store its strings.
		//(It needs its own datatype.)
		//daeAST::Data &d = dst[-daeOffsetOf(daeAST::Data,_union)];
		//assert(0!=d.getOffset()&&d.getIsXmlns());
		assert(_type(dst)==&daeAnySimpleType_xml);
		daeStringRef &xml = dst;
		xml.setString(srcIn,srcN);
		return DAE_OK;
	}
	default: string: //last resort

		COLLADA_SUPPRESS_C(4551)
		assert(!"_failure_to_unserialize");
		daeEH::Error<<"daeAST::AnyWriter::_failure_to_unserialize()\n"
		<<"PROGRAMMER ERROR: Unexpectedly reverting non-schema/xs:anySimpleType data to xs:string...\n"
		<<daeHashString(srcIn,std::min<size_t>(srcN,64));
		daeAnySimpleType_string._reset_type(dst,0);
		return daeAnySimpleType_string._atomWriter->unserialize(srcIn,srcN,dst).error;
	}
	promote->_reset_type(dst,size);
	if(!promote->_unionWriter->unserialize(srcIn,srcN,dst))
	goto string; return DAE_OK;
}

//---.
}//<-'

/*C1071*/
