/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */
#include <ColladaDOM.inl> //PCH

#ifdef __COLLADA_DOM__ZAE

//_DEQUESIZ is broken. Why even use STD library on MSVC??
#ifndef _MSC_VER
#include <deque>
#define daeZAE_vec std::deque<char>
#else
#define daeZAE_vec std::vector<char>
#endif

COLLADA_(namespace)
{//-.
//<-'

/**EXPERIMENTAL
 * daeGZ is a candidate for exposing to clients in order
 * help download GZ compressed files. E.g. when the HTTP
 * header says the content is GZ-compressed. It's common.
 */
class daeGZ
{		
	char _32k_dictionary[32*1024]; 
	char _private_implementation[daeZAEPlugin::_gz_implementation]; //4096*4
	size_t _restart,_final_input;
	size_t _size;
	#ifdef NDEBUG
	#error std::vector should be a daeArray with a built-in buffer.
	#endif
	std::vector<char> _data;
	daeError _error;

COLLADA_(public)

	daeGZ(size_t fin){ reset(fin); }	
	
	inline void reset(size_t fin)
	{
		_data.clear(); _size = 0;

		_restart = 0; _final_input = fin; _error = DAE_OK;
	}

	//SCHEDULED FOR REMOVAL? Maybe data()?
	template<class T> inline operator T*()
	{
		assert(!empty()); return &_data[0]; 
	}

	inline size_t size()const{ return _size; }

	inline bool empty()const{ return _size==0; }

	inline void clear()
	{
		_data.erase(_data.begin(),_data.end()-getUnderflow());
		_size = 0;
	}	
	
	inline size_t getUnderflow()const
	{
		return _data.size()-_size; 
	}
	//Reminer: It's tempting to remove this for symmetry with 
	//a write operation, where the amount of output is unknown.
	//But it's convenient to store it in daeGZ. And it doesn't
	//look like daeGZ will be able to do DEFLATE jobs anyway.
	inline size_t getRemaining()const
	{
		return _final_input-_restart-getUnderflow(); 
	}

	/**
	 * @return Returns @c size() or 0. If nonzero the stream should be
	 * in working order; If not then @c getError() should be consulted.
	 */
	COLLADA_DOM_LINKAGE size_t inflate(const void *in, size_t in_chars);
	//The Miniz "tdefl" implementation is several times larger than the
	//"tinfl" data structure. Not sure if they can be reconciled into 1
	//structure, or if they are, there's no reason to do it.
	/**
	 * @return Returns @c size() or 0.
	 */
	//COLLADA_DOM_LINKAGE size_t deflate(const void *in, size_t in_chars);
	
	/**
	 * @return Returns @c DAE_NOT_NOW if the DEFLATE stream is in an 
	 * incomplete state; i.e. more input/output is required in order
	 * to not lose informatin at the stream's end.
	 */
	inline daeError getError()const{ return _error; }

	inline bool isOK()const{ return _error==DAE_OK; }
};

/**INTERNAL
 *
 * TO-DO LIST
 * Enable encryption features?
 * Convert non-ASCII IBM437 paths to UTF8.
 * 64-bit structures.

 4.4.5 compression method: (2 bytes)

        0 - The file is stored (no compression)
        1 - The file is Shrunk
        2 - The file is Reduced with compression factor 1
        3 - The file is Reduced with compression factor 2
        4 - The file is Reduced with compression factor 3
        5 - The file is Reduced with compression factor 4
        6 - The file is Imploded
        7 - Reserved for Tokenizing compression algorithm
        8 - The file is Deflated
        9 - Enhanced Deflating using Deflate64(tm)
       10 - PKWARE Data Compression Library Imploding (old IBM TERSE)
       11 - Reserved by PKWARE
       12 - File is compressed using BZIP2 algorithm
       13 - Reserved by PKWARE
       14 - LZMA (EFS)
       15 - Reserved by PKWARE
       16 - Reserved by PKWARE
       17 - Reserved by PKWARE
       18 - File is compressed using IBM TERSE (new)
       19 - IBM LZ77 z Architecture (PFS)
       97 - WavPack compressed data
       98 - PPMd version I, Rev 1
 */
class daeZAE : public daeAtlas, daeAtlasValue
{
COLLADA_(public) //CENTRAL-DIRECTORY

	daeZAE(const daeIORequest &req):daeAtlas(req)
	,_initialized(){}
	bool _initialized; 
	enum{ _8192=8192 };
	daeError maybe_init(daeIO &IO);
	daeError _init_central_directory(daeIO*,char*,int,int,int);
	struct cde //VARIABLE-LENGTH
	{				
		//LOSSY! comments, etc. won't be written.
		unsigned isize,dsize,lhpos,mtime,dcode:8; daeStringCP path[1];

		static bool less(cde*a,cde*b){ return strcmp(a->path,b->path)<0; }
	};
	std::vector<cde*> CD; daeStringAlligator<cde,16> A;

	typedef std::vector<cde*>::const_iterator iterator;
		
	static cde *cde_cast(daeClientString p)
	{
		return p==nullptr?nullptr:(cde*)(p-daeOffsetOf(cde,path));
	}

COLLADA_(public) //daeAtlas methods
	
	virtual daeError getNames(daeArray<daeClientString>&,daeName)const;
	virtual daeError getValues(daeArray<int>&,daeClientString)const;
	
COLLADA_(public) //HELPER

	//EXPERIMENTAL
	//This logic is kind of a mess. I'm not smart enough to come up with a 
	//better arrangment. Maybe daeAtlas can facilitate this sort of thing?
	struct trio
	{
		daeIOPlugin *first;
		const daeArchive *second;		
		cde *third;
		trio(daeIOPlugin *p, daeZAE *a)
		:first(p),second(p->getRequest().scope),third()
		{
			if(nullptr!=second) 
			for(;second->isArchive();second=&second->getArchive()) 			
			if(a==&second->getAtlas())
			{
				third = cde_cast
				(a->name(*p->getRequest().localURI-second->getDocURI()));	
				return;
			}			
			//HACK: This I/O framework is not ideal in this hypothetical case.
			//Here either the I or O side is outside of the scope of this ZAE.
			second = p->getRequest().scope; 
		}		
	};

COLLADA_(public) //I/O
		
	struct rw : daeIOSecond<>
	{
		cde *e; 				
		int e_overflowed;
		//img==0: offset is ZIP header size.
		//img!=0: offset is its r/w pointer.
		size_t offset;
		daeImageRef img;
		daeIO *IO;			
		daeIORequest req;			
		rw(daeIO *IO=nullptr):e()
		,offset(),e_overflowed(-1),IO(IO) //piggybackI
		{}
		rw(const trio &p):daeIOSecond(req)
		,e(p.third),offset(),e_overflowed(-1),IO()
		,req(e==nullptr?nullptr:&p.second->getArchive())
		{				
			if(e!=nullptr) //Use daeImage?
			try_image(p.second->getAtlas()); 
				
			if(!req.isEmptyRequest())
			{
				req.remoteURI = //Default to localURI.
				req.localURI = &p.second->getDocURI();
				p.second->getAtlas()->getSource(req);
			}
		}
		void try_image(daeAtlas&, size_t i=-1);
		bool cpy_image(daeAtlas&, size_t i, daeIO *io);		
		void maybe_openIO(daeIOPlugin &I, daeIOPlugin &O)
		{
			if(!req.isEmptyRequest())
			IO = req.scope->getIOController().openIO(I,O);
		}
		daeOK maybe_closeIO(daeOK OK=DAE_OK)
		{ 
			if(IO!=nullptr //try_image?
			&&req.string.empty() //_stringI?
			&&!req.isEmptyRequest()) //piggybackI?			
			OK = req.scope->getIOController().closeIO(IO,OK);
			IO = nullptr; return OK;
		}
		~rw()
		{
			assert(IO==nullptr); //maybe_closeIO();

			if(img!=nullptr) img->atomize(); //HINT
		}
	};
	struct io : daeIO 
	{	
		rw r,w; 
		daeStringI _stringI;
		daeIOEmpty _emptyIO;
		daeOK OK; size_t lock;		
		io(const std::pair<trio,trio> &IO)
		:r(IO.first),w(IO.second)
		,_stringI(r.req.string),lock(-1)
		{
			if(!_stringI.empty())	
			{
				r.IO = &_stringI;
			}
			else r.maybe_openIO(r,_emptyIO);
			
			w.maybe_openIO(_emptyIO,w);
		} 		
		io(std::pair<daeIO*,cde*> I):r(I.first),lock(-1)
		{
			r.e = I.second; //piggybackI
		} 	

		//daeIO methods//////////////////////////

		virtual daeError getError(){ return OK; }
		virtual daeOK readIn(void*,size_t);
		virtual daeOK writeOut(const void*,size_t);
		virtual size_t getLock(Range *I, Range *O)
		{
			if(OK!=DAE_OK||lock!=size_t(-1))
			return lock;				
			lock = 0;
			if(r.e!=nullptr) //IIIIIIIIIIIIIIIIIIIII
			{	
				//ext is if reading from the back of 
				//the headers. est is estimated size.
				size_t ext,est; if(I==nullptr)
				{
					ext = r.e->dsize;
				}
				else if(0==I->first)
				{
					ext = std::min(r.e->dsize,I->second);
					I->second = ext;
				}
				else ext = 0; enum{extraN=64};
				est = ext+30+strlen(r.e->path)+extraN;
					
				Range dI = {0,est};
				dI.offset(r.e->lhpos);
				r.IO->setRange(&dI,nullptr);
				char buf[4096]; 
				if(!r.IO->readIn(buf,30))
				{
					OK = DAE_ERR_BACKEND_IO;
					return 0;
				}
									  					
				//Read to end of ZIP's headers.
				int gpbits = BigEndian<16,unsigned short>(buf+6);
				int method = BigEndian<16,unsigned short>(buf+8);
				int rem = BigEndian<16,unsigned short>(buf+26);
				int extra = BigEndian<16,unsigned short>(buf+28);
				rem+=extra; assert(extra<=extraN);
				int reset = r.offset = 30+rem;

				if(ext!=0) if(est<reset+ext) //HACK?
				{
					//The estimate was inadequate.
					//HACK: Assuming not interested in Extra field.
					dI.first+=reset;
					dI.second = dI.first+ext;
					r.IO->setRange(&dI,nullptr);						
					daeEH::Warning<<"daeZAEPlugin's Extra data allotment ("<<+extraN<<") is inadequate. Resetting download range. Extra was "<<extra;
				}
				else for(size_t rd;rem!=0;r.IO->readIn(buf,rd),rem-=rd)
				{
					rd = std::min<int>(rem,sizeof(buf));
				}
					
				if(OK=r.IO->getError()) switch(method)
				{
				default:
							
					daeEH::Error<<"ZIP decompression method ("<<method<<") is unsupported.";
							
					OK = DAE_ERR_NOT_IMPLEMENTED; break;

				case 8: r.e_overflowed = 0;

					gz = zae->gz_stack.pop(r.e->dsize); //break;

				case 0: lock = r.e->isize; break;
				}						
				if(ext==0) setRange(I,nullptr);
			}
			else if(r.img!=nullptr)
			{
				if(r.img->getIsDeleted())
				{
					OK = DAE_ERR_BACKEND_IO; //Or???
					daeEH::Warning<<"Reading deletion from atlas ("<<r.img->getName()<<")";
				}
				else lock = r.img->size();

				if(I!=nullptr) setRange(I,nullptr);
			}
			else if(r.IO!=nullptr) //Pass-thru?
			{
				lock = r.IO->getLock(I,nullptr);
			}
			if(w.e!=nullptr) //OOOOOOOOOOOOOOOOOOOOO
			{
				assert(O==nullptr); //incomplete

				#ifdef NDEBUG
				#error incomplete
				#endif
				OK = DAE_ERR_NOT_IMPLEMENTED;
			}
			else if(w.IO!=nullptr) //Pass-thru?
			{
				w.IO->getLock(nullptr,O);
			}
			return lock;
		}
		virtual size_t setRange(Range *rngI, Range *rngO)
		{
			if(lock==size_t(-1)) return getLock(rngI,rngO);
			
			if(rngI!=nullptr) 
			{
				rngI->limit_to_size(lock);
				bool stored = 0>r.e_overflowed;
				bool partial = 0!=rngI->first||lock!=rngI->second;
				if(!stored&&partial)
				{					
					//Making a random-access copy.
					size_t i;
					if(!zae->findImage(r.e->path,i))
					if(!r.cpy_image(*zae,i,this))
					{
						OK = DAE_ERR_BACKEND_IO;
						rngI->limit_to_size(0);
						return lock;
					}
				}
				if(r.img!=nullptr) //Image?
				{
					r.offset = rngI->first;
				}
				else //Not partial or uncompressed?
				{
					daeIO::Range dI = {r.offset+rngI->first};
					dI.second = stored?r.offset+rngI->second:r.e->dsize;
					dI.offset(r.e->lhpos);
					r.IO->setRange(&dI,nullptr);
				}
			}
			if(rngO!=nullptr)
			{
				OK = DAE_ERR_NOT_IMPLEMENTED; assert(0);
				rngO->limit_to_size(0); 
			}

			return lock;
		}		

		//daeIOController methods ////////
		
		daeSmartRef<daeZAE> zae; daeGZ *gz;
									
		io *set_zae(daeZAE *ioc)
		{
			zae = ioc; gz = nullptr; return this;
		}

		~io(){ if(gz!=nullptr) zae->gz_stack.push(gz); }

		daeOK close(daeOK cancel)
		{
			//Reminder: It's important to close the read
			//file first so its write-lock doesn't block
			//self-overwriting operations.
			daeOK rOK = r.maybe_closeIO(cancel);
			daeOK wOK = w.maybe_closeIO(cancel);
			return OK?wOK?rOK:wOK:OK;
		}
	};

COLLADA_(public) //daeIOController methods
	
	//static just to reduce outstanding memory.
	static daeIOController::Stack<io> io_stack;
	static daeIOController::Stack<daeGZ> gz_stack;

	/**PURE-OVERRIDE */
	daeIO *openIO(daeIOPlugin &I, daeIOPlugin &O)
	{
		return io_stack.pop(std::make_pair(trio(&I,this),trio(&O,this)))->set_zae(this);
	}
	/**PURE-OVERRIDE */
	daeOK closeIO(daeIO *IO, daeOK cancel)
	{
		daeOK OK = ((io*)IO)->close(cancel); io_stack.push(IO); return OK;
	}

COLLADA_(public) //INTERNAL

	inline daeIO *piggybackI(daeIO &IO, daeName path)
	{
		cde *e = cde_cast(name(path));
		return e==nullptr?nullptr:io_stack.pop(std::make_pair(&IO,e))->set_zae(this);
	}
};
daeZAE::Stack<daeZAE::io> daeZAE::io_stack;
daeZAE::Stack<daeGZ> daeZAE::gz_stack;

daeError daeZAE::maybe_init(daeIO &IO)
{
	if(this==nullptr)
	{
		assert(this!=nullptr);
		return DAE_ERR_BACKEND_IO;
	}
	if(!_initialized) _initialized = true;
	else return DAE_OK;

	//ZIP files (which ZAE is) are organized in reverse.
	//Conceptually eof is requesting the Content-Length.
	daeIO::Range r = {-1,-1};
	size_t eof = IO.getLock(&r); if(eof!=0) 
	{
		r.first = std::max<size_t>(eof,_8192)-_8192; 
		r.second = eof;
		if(eof==IO.setRange(&r))
		{
			size_t bufN = r.second-r.first;
			char buf[_8192];
			daeOK OK = IO.readIn(buf,bufN);
			if(!OK) return OK;

			//HACK: Just throwing this together.
			int sig = 0x06054b50; BigEndian<32>(sig);
			char *eocd = buf+bufN-sizeof(sig);
			while(eocd>=buf&&*(int*)eocd!=sig) 
			eocd--; 
			size_t eocdN = bufN-((char*)eocd-buf);
			if(eocd>buf&&eocdN>=22)
			{
				#ifdef NDEBUG
				#error 64bit ZIP?
				#endif
				daeCTC<CHAR_BIT==8>();
				int n = BigEndian<16,unsigned short>(eocd+8);
				int cd = BigEndian<32,unsigned int>(eocd+12);
				if(eocd-cd<buf)
				{
					r.first = r.second = eof-eocdN;
					if((size_t)cd>=r.first)
					{
						r.first-=cd;
						if(eof==IO.setRange(&r))
						{
							return _init_central_directory(&IO,buf,0,cd,n);
						}
						else assert(0);
					}
					else daeEH::Error<<"Found ZIP central-directory is out-of-bounds.";
				}	
				else return _init_central_directory(nullptr,eocd-cd,cd,0,n);
			}
			else daeEH::Error<<"Did not find ZIP signature (06054b50) in last "<<_8192<<"Bs.";
		}
		else assert(0);
	}
	return DAE_ERR_BACKEND_IO;
}
daeError daeZAE::_init_central_directory(daeIO *IO, char *buf, int bufN, int rem, int reserve)
{
	CD.reserve(reserve);

	char *cdp,*eob; int rd;
	if(IO==nullptr) goto short_cd; do
	{
		bufN = buf+bufN-cdp;
		rd = std::min(rem,_8192-bufN);
		memmove(buf,cdp,bufN);
		IO->readIn(buf+bufN,rd); bufN+=rd; rem-=rd;
		
		short_cd: cdp = buf; eob = buf+bufN; for(;cdp+46<eob;)
		{
			assert((0x02014b50==BigEndian<32,int>(cdp)));			
			short gpbits = BigEndian<16,short>(cdp+8); 
			short method = BigEndian<16,short>(cdp+10);
			if(8!=method&&0!=method)
			{
				daeEH::Error<<"Unsupported ZIP encoding: "<<method;
				goto error;
			}

			short pathlen = BigEndian<16,short>(cdp+28);
			if(cdp+46+pathlen>eob) break;

			int eN = sizeof(cde)+pathlen;
			if(A._nexT->path+pathlen>=A._end)
			A._reserve(std::max(eN,A._reserveN()));
			cde &e = *A._nexT; A._nexT+=eN;			
			
			e.dcode = 0x7F&method;
			e.mtime = BigEndian<32,unsigned int>(cdp+12);
			e.dsize = BigEndian<32,unsigned int>(cdp+20);
			e.isize = BigEndian<32,unsigned int>(cdp+24);
			e.lhpos = BigEndian<32,unsigned int>(cdp+42);
			if(0==(gpbits&0x400))
			{
				//TODO? IBM437 to UTF8.
			}
			memcpy(e.path,cdp+46,pathlen);
			e.path[pathlen] = '\0';

			int extra = BigEndian<16,unsigned short>(cdp+30);
			int comment = BigEndian<16,unsigned short>(cdp+32);
									   
			CD.push_back(&e); cdp+=46+pathlen+extra+comment;
		}
	
	}while(rem>0); std::sort(CD.begin(),CD.end(),cde::less);

	return IO!=nullptr?IO->getError():DAE_OK;

	error: CD.clear(); return DAE_ERROR;
}

daeOK daeZAEPlugin::addDoc(daeDocRef &add, daeMeta *rootmeta)
{	
	const daeIORequest &req = getRequest();

	daeZAE *zae = new daeZAE(req);
	zae->__DAEP__Object__unembed();
	daeArchiveRef out(*req.getDOM());		
	out->_getAtlas() = zae;

	//This is not pretty. Basically, assuming the caller means
	//to open the index document as daeArchive::_read2 does it.
	if(rootmeta!=nullptr
	&&req.localURI!=nullptr
	&&req.localURI->getURI_extensionIs("zae"))
	{
		//This branch continues to daeZAEPlugin::readContent().
		out->getDocURI() = *req.localURI;
		out->setDocument(out->newDoc("dae_root"));
	}
	else //HACK: Open any ZIP without dae_root or manifest.xml?
	{			
		#ifdef NDEBUG
		#error Can (or should) this wait until it's called for?
		#endif

		//This is what _read2 would do, but there isn't an API
		//that is analogous to daeIOPlugin::readContent.
		daeIOEmpty O;
		daeRAII::CloseIO RAII(req.scope->getIOController()); //emits error	
		daeStringI strI(req.string);
		daeIO *IO = nullptr;
		if(strI.empty()) IO = RAII = RAII.p.openIO(*this,O);
		else IO = &strI;
		if(IO==nullptr)
		return DAE_ERR_BACKEND_IO;
		daeOK OK = zae->maybe_init(*IO);
		if(!OK) return OK;
	}
	add = out; return DAE_OK;
}
daeOK daeZAEPlugin::readContent(daeIO &IO, daeContents &content)
{
	//Assuming this means the archive is being opened, since it is
	//hard to imagine doing a partial read into the index document.

	daeDocument *d = (daeDocument*)content.getElement().getDoc();
	daeZAE *zae = dynamic_cast<daeZAE*>(&d->getArchive().getAtlas());
	daeOK OK = zae->maybe_init(IO); if(!OK) return OK;
	
	daeIO *zae_IO = zae->piggybackI(IO,"manifest.xml");
	if(zae_IO!=nullptr) //dae_root?
	{
		daeURI localURI; //dae_root

		size_t eof = zae_IO->getLock();			
		char *manifest_xml = new char[eof]; 
		OK = zae_IO->readIn(manifest_xml,eof); 					
		if(OK&&eof!=0)
		{
			//Here daeZAEPlugin could generate a full daeDocument
			//just to pluck out the dae_root element. It doesn't
			//have to be the final word on ZAE/archive plugins.
			//users are better to open manifest.xml themself.
			manifest_xml[eof-1] = '\0'; 
			char *e,*p = strstr(manifest_xml,"<dae_root"); 
			if(p!=nullptr) p = strchr(p,'>');
			if(p!=nullptr) e = strchr(++p,'<'); 
			if(p!=nullptr&&e!=nullptr)					
			{
				while(COLLADA_isspace(*p)) p++;
				while(COLLADA_isspace(e[-1])) e--; *e = '\0'; 

				if(e>p) localURI.setURI(p); //dae_root?
			}
		}	
		delete[] manifest_xml;		
		
		//NOTE: piggybackI would work here, except replicating _read2's
		//plugin selection logic should be avoided.
		//TODO? encapsulate the plugin selection logic into some struct.
		//(Especially because IO is not yet closed.)
		if(!localURI.empty())
		{				
			//HACK: _read2 is recursive here so that the document can be
			//returned by the outer _read2.
			daeDocRef d2(d);
			if(getRequest().remoteURI!=getRequest().localURI) //unlikely
			{
				//Should daeAtlas hold onto the original remote URI?
				daeURI_parser remoteURI_base(getRequest().remoteURI->data());
				remoteURI_base.setIsDirectoryLike();
				remoteURI_base.setIsResolved();
				daeURI remoteURI(remoteURI_base,localURI.data());
				OK = daeArchive::_read2(d2,d->getMeta(),
			daeIORequest(&d->getArchive(),nullptr,&localURI,&remoteURI),_I);
			}else OK = daeArchive::_read2(d2,d->getMeta(),
			daeIORequest(&d->getArchive(),nullptr,&localURI,&localURI),_I);				
		}
		else daeEH::Warning<<"ZAE is without <dae_root> via manifest.xml.";
	}
	else daeEH::Warning<<"ZAE is without manifest.xml.";

	if(!d->getDocURI().empty())
	{			
		if(getRequest().localURI!=nullptr) //Overrules dae_root fragment?
		{
			daeRefView fragment = getRequest().localURI->getURI_fragment();
			if(!fragment.empty()) d->getFragment() = fragment;
		}
	}
	else //d->close(); //Should it be empty or missing?
	{		
		//This should close it. But what about the ZAE archive itself?
		OK = DAE_ERR_BACKEND_IO;
	}

	return OK;
}

/**
 * This is used when a compressed subimage is accessed in its middle.
 * Mainly this is for nested ZIP files since they are read from back 
 * to front; and it doesn't seem that DEFLATE can jump to the middle.
 */
struct daeZAE_inflated : daeImage
{
	mutable daeZAE_vec _data; //std::deque<char>
		
	virtual size_t data(const void *in, daeIO::Range *r1, daeIO::Range *r2)const
	{
		daeZAE_vec::iterator it = _data.begin();

		if(r2!=nullptr) //Read or Erase?
		{
			if(in!=nullptr&&r1==nullptr) //Read?
			{
				//COLLADA_SUPPRESS_C(4996)
				std::copy(it+r2->first,it+r2->second,(char*)in);
				return _data.size();
			}
			_data.erase(it+r2->first,it+r2->second);
		}

		if(r1!=nullptr) if(in!=nullptr) //Insert?
		{
			_data.insert(it+r1->first,(char*)in,(char*)in+r1->size());
		}
		else _data.insert(it+r1->first,r1->size(),0); return _data.size();
	}

	daeZAE_inflated(daeClientString name):daeImage(name){}
};
bool daeZAE::rw::cpy_image(daeAtlas &a, size_t i, daeIO *io)
{		
	daeZAE_inflated *o = new daeZAE_inflated(e->path); 
	o->__DAEP__Object__unembed();
		
	daeIO::Range r = {0,e->isize};
	size_t rem = io->setRange(&r,nullptr);
	char buf[_8192];
	for(size_t rd;rem>0;rem-=rd)
	{
		rd = std::min(rem,sizeof(buf));
		if(!io->readIn(buf,rd)) break;
		o->_data.insert(o->_data.end(),buf,buf+rd);
	}
	if(rem!=0)
	{
		assert(rem==0); return false;
	}

	//DICEY: Change over to image based I/O.
	offset = 0; e_overflowed = -1;

	a.contain(o,i); try_image(a,i); return true;
}
void daeZAE::rw::try_image(daeAtlas &a, size_t i)
{
	if(i==-1&&!a.findImage(e->path,i)) return;

	img = a.getContained()[i]; 

	//DICEY: Change over to image based I/O.
	maybe_closeIO(); e = nullptr; req.scope = nullptr;	
}



////READING ////READING ////READING ////READING ////READING ////READING
////READING ////READING ////READING ////READING ////READING ////READING
////READING ////READING ////READING ////READING ////READING ////READING



//daeGZ.cpp is defining these minimal extern APIs.
typedef struct _tinfl_decompressor tinfl_decompressor;
extern unsigned *get_tinfl_decompressor_state_etc(void*,size_t,size_t);
extern daeError tinfl_decompress(void*,const void*,size_t*,void*,void*,size_t*);
size_t daeGZ::inflate(const void *in, size_t in_chars)
{	
	if((int)_error<0) return 0;

	//daeCTC<(sizeof(_private_implementation)>=sizeof(tinfl_decompressor))>();
	tinfl_decompressor &decomp = (tinfl_decompressor&)_private_implementation;
	unsigned *st = get_tinfl_decompressor_state_etc(&decomp,sizeof(_private_implementation),1);	
	if(_restart==0) 
	{	
		st[0] /*decomp.m_state*/ = 0; //tinfl_init(&decomp);

		//These are extensions to the original Miniz data structure.
		//Really the 32k dictionary should be housed inside it also.
		st[1] /*decomp.ext.wrapping_output_buf*/ = 0;
	}
	unsigned &ext_wrapping_output_buf = st[1]; 
	
	//THIS IS really all one big loop. 
	//tinfl_decompress expects _32k_dictionary to be a persistent
	//circular buffer, which really complicates things when input
	//is partially buffered.			
	char buf[4096];	
	size_t underflow = getUnderflow();
	assert(underflow<sizeof(buf));
	if(0!=underflow)
	memcpy(buf,&_data[_size],underflow);
	_data.resize(_size); 	
	//f is almost no longer needed.
	int f = 1; //TINFL_FLAG_HAS_MORE_INPUT
	for(bool looping=true;looping;)
	{
		//TODO? In theory buf can be discarded as soon as carried
		//overflow is consumed. Whether it can perform any better 
		//or not, it seems easier to conceptualize this algorithm.
		const size_t cp = std::min(sizeof(buf)-underflow,in_chars);
		const size_t bufN = underflow+cp;		
		memcpy(buf+underflow,in,cp);
		(char*&)in+=cp; in_chars-=cp;
			 	
		size_t rem = _final_input-_restart;
		if(bufN>=rem) if(bufN!=rem)
		{
			_error = DAE_ERR_INVALID_CALL; return 0;
		}
		else f = 0;
		looping = in_chars!=0||f==0&&underflow!=0;

		underflow = bufN; //Misleading...
		char *offset = _32k_dictionary+ext_wrapping_output_buf;
		size_t out = sizeof(_32k_dictionary)-ext_wrapping_output_buf;
		_error = tinfl_decompress(&decomp,buf,&underflow,_32k_dictionary,offset,&out/*,f*/);	
		underflow = bufN-underflow;
		const size_t bufN_underflow = bufN-underflow;
		
		ext_wrapping_output_buf+=out;
		if(ext_wrapping_output_buf>=sizeof(_32k_dictionary))
		{
			ext_wrapping_output_buf-=sizeof(_32k_dictionary);
			assert(0==ext_wrapping_output_buf);
		}

		_restart+=bufN_underflow;
	
		switch(_error)
		{
		//case TINFL_STATUS_DONE: 
		case DAE_OK: looping = false;		
		//case TINFL_STATUS_HAS_MORE_OUTPUT:
		//case TINFL_STATUS_NEEDS_MORE_INPUT:
		case DAE_NOT_NOW: 
			
			//Worried about returning 0+DAE_NOT_NOW.
			//Might want to go back to returning OK.
			assert(out!=0||_error==DAE_OK);
			break;

		default: _error = DAE_ERR_BACKEND_IO; return 0; 
		}
				
		_data.insert(_data.end(),offset,offset+out);	
		_size = _data.size();
		if(underflow!=0) 
		if(looping) memmove(buf,buf+bufN_underflow,underflow);
		else _data.insert(_data.end(),buf+bufN_underflow,buf+bufN);
	}															   			
	return _size;
}
daeOK daeZAE::io::readIn(void *in, size_t chars)
{
	bool coded = r.e_overflowed>=0;

	if(coded&&!gz->empty()) inflated:
	{
		size_t cp = gz->size()-r.e_overflowed;
		if(cp!=0)
		{
			cp = std::min<size_t>(chars,cp);
			memcpy(in,(char*)*gz+r.e_overflowed,cp); 
			r.e_overflowed+=(int)cp;								
			(char*&)in+=cp;
			chars-=cp; 					
		}
		if(r.e_overflowed==(int)gz->size())
		{
			r.e_overflowed = 0;
			gz->clear();
		}

		if(0==chars) return OK;
	}

	char *buf = (char*)in, underflow[512];
	size_t charz = chars; if(coded)
	{
		//This is just to keep the output buffer
		//from being very large when reading in 
		//an entire file. Maybe it should depend
		//on the ratio of dsize and isize.
		charz = std::min<size_t>(charz,4*4096);

		charz = std::max(charz,sizeof(underflow));				
		charz = std::min(charz,gz->getRemaining());				
		if(charz>chars) buf = underflow;
	}

	if(OK) if(r.img!=nullptr)
	{
		//Note: Overflow is "undefined behavior."
		size_t to = r.offset+chars;
		if(to<=r.img->copy(r.offset,to,buf))
		r.offset = to;
		else OK = DAE_ERR_INVALID_CALL;	
	}
	else if(OK=r.IO->readIn(buf,charz))
	{
		if(coded)
		{
			if(0==gz->inflate(buf,charz))
			{
				OK = gz->getError(); assert(OK!=DAE_NOT_NOW);
			}
			else goto inflated;
		}
	}

	return OK;			
}		



////WRITING ////WRITING ////WRITING ////WRITING ////WRITING ////WRITING
////WRITING ////WRITING ////WRITING ////WRITING ////WRITING ////WRITING
////WRITING ////WRITING ////WRITING ////WRITING ////WRITING ////WRITING
		 


//daeGZ.cpp is defining these minimal extern APIs.
extern void *new_tdefl_compressor(bool(*flush)(const void*,size_t,void*),void*,void*init_or_delete);
extern daeError tdefl_compress(void*,const void *in=nullptr,size_t chars=0);
extern int tdefl_crc32(const unsigned char*,size_t,unsigned=0);

daeOK daeZAE::io::writeOut(const void *out, size_t chars)
{
	#ifdef NDEBUG
	#error should this write to daeZAE_inflated???
	#endif
	assert(0); return OK = DAE_ERR_NOT_IMPLEMENTED;
}

daeError daeZAE::getNames(daeArray<daeClientString> &o, daeName wc)const
{
	daeError err = DAE_OK;
	size_t i = 0, m = CD.size(), n = m; if(wc!=nullptr)
	{
		cde *v = cde_cast(wc.string);
		i = m = std::lower_bound(CD.begin(),CD.end(),v,cde::less)-CD.begin();
		//&[0]: MSVC sees path as a 1-sized container (char[1].)
		while(m<n&&std::equal(wc.begin(),wc.end(),&CD[m]->path[0]))
		m++;		
		if(m-i!=1||'\0'!=CD[i]->path[wc.extent]) //Exact match?
		err = DAE_ERR_QUERY_NO_MATCH;
	}
	for(;i<m;i++) o.push_back(CD[i]->path); return err;
}	 	
daeError daeZAE::getValues(daeArray<int> &values, daeClientString named)const
{
	if(named==nullptr) return DAE_ERR_NOT_IMPLEMENTED;
	
	if(values.empty())
	{
		int d[] = {CODE,0,SMALL,0,LARGE,0,TIME,0};
		values.assign(d,sizeof(d)/sizeof(*d));
	}
	
	cde *e = cde_cast(named);

	//REMINDER: SMALL/LARGE do not refer to daeImage backed 
	//names. Callers must use daeImage::size() to determine
	//their size, or @c daeIO::getLock().
	for(size_t i=0;i<values.size();i+=2) 
	{
		unsigned &v = *(unsigned*)&values[i+1]; 
		switch(values[i])
		{
		case CODE: v = e->dcode; break;
		case SMALL: v = e->dsize; break;
		case LARGE: v = e->isize; break;
		case TIME: v = e->mtime; break;
		}
	}
	return DAE_OK;
}

/**INTERNAL (HELL-ON-EARTH)
 * Implements @c daeZAEPlugin::writeRequest().
 *
 * @warning in 15 years of programming I've never had a more
 * miserable time than I have thinking about & planning this.
 * To add insult to injury, @c Zipper only facilitates basic
 * usage (archive->ZAE/ZIP.)
 */
class daeZAEPlugin::Zipper : daeAtlasValue 
{		
	daeZAEPlugin &in; 
	daeIO &io;
	daeZAE_vec cd; //std::deque<char>		
	void *gz;

	//write //write //write //write

	struct zipIO : daeIO //I/O base & CRC-32 logic.
	{	
		daeOK OK; 
		
		Zipper &z;

		int checksum;

		size_t checksize;				
		
		zipIO(Zipper &z):z(z),checksum(),checksize(){}

		virtual daeError getError()
		{
			return OK.error; 
		}	
		virtual size_t getLock(Range *I, Range *O)
		{
			assert(I==nullptr&&O==nullptr);
			return 0;
		}
		virtual size_t setRange(Range *I, Range *O)
		{
			return getLock(I,O);
		}
		virtual daeOK writeOut(const void *in, size_t chars)
		{
			_write(in,chars); return OK;
		}		
		void _write(const void *in, size_t check)
		{
			checksum = tdefl_crc32((unsigned char*)in,check,checksum);
			checksize+=check;
			if(OK) crc32_write(in,check);
		}		

		virtual void crc32_write(const void *in, size_t chars)
		{
			//Reminder: Inner archives use this implementation.
			//z.io is correct (not z.crc) otherwise this would
			//be a pure-virtual method.
			OK = z.io.writeOut(in,chars);
		}
	};
	struct storedIO : zipIO //Uncompressed.
	{
		size_t smallsize; bool incomplete;

		storedIO(Zipper &z):zipIO(z),smallsize(),incomplete()
		{}
		~storedIO(){ assert(!incomplete); }

		virtual void crc32_write(const void *in, size_t chars) 
		{
			smallsize+=chars; OK = z.crc->writeOut(in,chars);
		}

		daeOK complete() //deflatedIO
		{
			if(incomplete)
			{
				incomplete = false;
				if(OK) OK = tdefl_compress(z.gz); //flush op
			}
			return OK;
		}
	};
	struct deflatedIO : storedIO //DEFLATE compressed.
	{
		deflatedIO(Zipper &z, int defIO=0):storedIO(z)
		{
			if(0==defIO) //deferredIO
			{
				incomplete = true;
				z.gz = new_tdefl_compressor(tdefl_func,this,z.gz);
			}
		}

		virtual void crc32_write(const void *in, size_t chars) 
		{
			//Reminder: tdefl_compress can set OK.
			daeOK defOK = tdefl_compress(z.gz,in,chars);
			if(OK) OK = defOK;
		}

		static bool tdefl_func(const void *in, size_t chars, void *w)			
		{	
			((deflatedIO*)w)->storedIO::crc32_write(in,chars);
			return true;
		}
	};
	struct deferredIO : deflatedIO //Maybe DEFLATE compressed.
	{
		deferredIO(Zipper &z):deflatedIO(z,1){}

		std::vector<char> stored,deflated;

		virtual void crc32_write(const void *in, size_t chars) 
		{
			stored.insert(stored.end(),(char*)in,(char*)in+chars);
			deflatedIO::crc32_write(in,chars);
		}		

		static bool tdefl_func(const void *in, size_t chars, void *w)			
		{
			std::vector<char> *deflated = (std::vector<char>*)w;
			deflated->insert(deflated->end(),(char*)in,(char*)in+chars);
			return true;
		}
		 
		void reset(int small, int large)
		{
			OK = DAE_OK; checksum = 0;			
			checksize = smallsize = 0; 

			deflated.reserve(small); stored.reserve(large); 

			z.gz = new_tdefl_compressor(tdefl_func,&deflated,z.gz);
		}
		float ratio()
		{
			if(OK) OK = tdefl_compress(z.gz); //flush op
			return (float)deflated.size()/stored.size();
		}
		int clear(int method)
		{	
			//POINTLESS? deflated needs to be flushed but
			//callers wouldn't know which method to use if
			//it's not already been flushed.
			if(OK) OK = tdefl_compress(z.gz); //flush op

			std::vector<char> &sel = method==0?stored:deflated;				 
			int ret = (int)sel.size();			

			if(OK&&0!=ret) if(ret!=sel.size()) //ZIP64?
			{	
				//TODO: daeZAE_HUGE_file_error(); 
				assert(0); OK = DAE_ERR_NOT_IMPLEMENTED;
			}
			else OK = z.crc->writeOut(&sel[0],sel.size());			

			deflated.clear(); stored.clear(); return ret;
		}

		bool empty(){ return stored.empty(); }

	}defIO; char buf[8192]; daeURI uri;

	daeOK write(daeIO &dest, const daeArchive &a, daeString path)
	{	
		daeOK OK = DAE_ERR_BACKEND_IO;
		uri.setURI(a.getDocURI(),path);
		uri.setIsResolved();
		daeIORequest req(&a,nullptr,&uri,&uri);		
		daeIOSecond<> I(req); daeIOEmpty O;		
		daeRAII::CloseIO RAII(a.getIOController());
		daeIO *inflate = RAII.p.openIO(I,O);
		if(inflate!=nullptr)
		{
			size_t rem = inflate->getLock();
			for(size_t rd;rem>0;rem-=rd)
			{
				rd = std::min(rem,sizeof(buf));
				if(!inflate->readIn(buf,rd)) break;
				dest.writeOut(buf,rd);
			}
			OK = inflate->getError();
			if(OK) OK = RAII.close();
		}
		return !OK?OK.error:dest.getError();
	}

	//iter //iter //iter //iter //iter

	struct doc //support
	:
	std::pair<daeString,const daeDoc*>
	{
		bool operator<(const doc &cmp)const
		{
			return strcmp(first,cmp.first)<0;
		}
	};

	std::vector<doc> docs;

	daeArray<daeClientString> directory;

	//THOUGHTS
	//Iterating over docs/atlas entries can be 
	//complicated. Can this be simplified to a
	//point that it can be offered by a header?
	//Zipper's members must be templates if so.
	struct iter : daeName 
	{
		Zipper &z;		
		size_t i,idoc,_ipop,j,_jpop;		
		iter(Zipper &z, const daeArchive &a)
		:z(z)
		,i(z.directory.size()),_ipop(i),idoc(i)
		,j(z.docs.size()),_jpop(j)
		{
			daeAtlas &atlas = a.getAtlas();
			atlas.getNames(z.directory);		
			z.directory.push_back(nullptr); //SENTINEL
			
			z.docs.insert(z.docs.begin()+j
			,a.getDocCount()+1,Zipper::doc()); //+1 SENTINEL

			//This is mainly for sorting, but more importantly
			//it's to efficiently solve a double-entry problem.			
			for(size_t k=0,jN=z.docs.size()-1;j<jN;j++,k++)
			{
				const daeDoc *d = a.getDoc(k);
				daeRefView diff = d->getDocURI()-a.getDocURI();
				daeClientString name = atlas.name(diff);
				doc().first = nullptr!=name?name:diff.view;
				doc().second = d;
			}
			j = _jpop;
			std::sort(z.docs.begin()+j,z.docs.end()-1);

			_advance_idoc(); 			
			daeStringCP tmp = '\0';
			daeName::string = &tmp; iter::operator++(0);
		}
		~iter()
		{
			z.directory.setCountLess(_ipop);
			z.docs.erase(z.docs.begin()+_jpop,z.docs.end());
		}		
		Zipper::doc &doc()
		{
			return z.docs[j];
		}
		daeClientString &name()
		{
			return z.directory[i];
		}
		void operator++(int)
		{
			assert(nullptr!=string);

			//Hack: delay increment so that
			//callers can make use of i & j.
			if(daeName::string==name())
			i++;
			if(daeName::string==doc().first) 
			{
				j++; _advance_idoc();
			}

			//When i gets to idoc either there are docs
			//that remain, or it's reached the last entry.
			daeName::operator=(i==idoc?doc().first:name());
		}
		static bool _less(daeString a, daeString b)
		{
			return strcmp(a,b)<0;
		}
		void _advance_idoc()
		{
			if(nullptr!=doc().first) //SENTINEL
			{ 
				/*Could try with small directories?
				for(size_t k=idoc;k<z.directory.size();k++)
				if(doc().first==z.directory[k])
				{
					idoc = k; return;
				}*/
				idoc = std::lower_bound(z.directory.begin()+i
				,z.directory.end()-1-i,doc().first,_less)
				-z.directory.begin();
			}
			else idoc = z.directory.size();
		}
	};

	//zip //zip //zip //zip //zip //zip 

	daeIO *crc; int _now; int now()
	{
		if(_now!=0) return _now;
		time_t utc; time(&utc); tm &t = *gmtime(&utc);
		return _now = 
		//(daeAtlasValue::decomposeTIME in reverse.)
		t.tm_year-80<<25|t.tm_mon+1<<21|t.tm_mday<<16
		|t.tm_hour<<11|t.tm_min<<4|t.tm_sec/2;
	}

	daeOK output_local_entries(const daeArchive &a, char eocd[22])
	{	
		//Reminder: 8 is a 2B field; int can detect 65535 overflow.
		int &eocd_entered = (int&)eocd[8]; //short&
		int &eocd_wrote2 = (int&)eocd[16]; 

		daeOK OK;
		enum{ vlistN=5*2 };
		const int vlist[vlistN] = {CODE,0,TIME,0,SMALL,0,LARGE,0,DELETE,0};
		daeArray<int,vlistN> values; 
		values.assign(vlist,vlistN);
		daeAtlas &atlas = a.getAtlas();
		size_t j = 0; daeClientString jmg = atlas.nextImage(nullptr,j);
		for(iter i(*this,a);i.string!=nullptr;i++)
		{
			//Presently there is no modification-time framework
			//for docs. Users would have to use setValue.
			//(Not that TIME's DOS timestamp is a good standard.)
			if(i.string==i.name())
			{
				//DELETION?
				//Can't wait for the I/O operation to fail since
				//the header is outputted first, in a continuous
				//stream. (setRange on write is virgin territory.)
				if(i.string==jmg)
				{
					daeImage *img = atlas.getContained()[j];
					atlas.nextImage(jmg,j);
					if(img->getIsDeleted())
					continue;
				}
				atlas.getValues(values,i.string);
				if(0!=values[9]) //DELETE
				continue;
			}
			else values[3] = now(); //TIME

			int dt = 0;
			//NOTE: daeAtlasValue doesn't have a CHECKSUM enum.
			//Here the compressed data could be copied directly
			//as an optimization in many cases, except checksums
			//are based on the uncoded image. A CHECKSUM enum can
			//be added, but it would not solve the general problem.
			//
			// So to make things simple, the data is taken uncoded
			// from which a checksum is generated. This may mean it
			// is decoded only to be recoded, but is straightforward.
			if(i.string!=i.doc().first)
			{	
				defIO.reset(values[5],values[7]); //SMALL,LARGE
				if(OK=write(defIO,a,i.string))
				values[1] = defIO.ratio()>0.80f?0:8; //CODE
				else return OK;
			}
			else //DOCUMENT/ARCHIVE
			{
				dt = i.doc().second->getDocType();
				assert(dt!=0);				
				values[1] = dt==daeDocType::ARCHIVE?0:8; //CODE
			}
			
			struct padding //align8
			{
				char pad[8];
				char lh[30];
			}lh_padded = {{},"PK\x03\x04"};
			char (&lh)[30] = lh_padded.lh;
			daeZAE::BigEndian<16>((short&)lh[4]=63); //UTF-8 (6.3)
			daeZAE::BigEndian<16>((short&)lh[6]=0x408); //UTF/"PK\x06\x07"
			daeZAE::BigEndian<16>((short&)lh[8]=values[1]); //CODE
			daeZAE::BigEndian<32>((int&)lh[10]=values[3]); //TIME			
			daeZAE::BigEndian<16>((short&)lh[26]=(short)i.extent);

			//Trying to facilitate mmap.
			int header = 30+(int)i.extent;
			int align8 = eocd_wrote2+header&8-1;
			if(align8!=0) align8 = 8-align8;

			crc->writeOut(lh-align8,align8+30);
			crc->writeOut(i.string,i.extent);		

			int checksum = 0;
			if(!defIO.empty()) 
			{
				assert(dt==0);				
				values[5] =	defIO.clear(values[1]); //SMALL,CODE
				OK = defIO.OK;
				values[7] = (int)defIO.checksize; //LARGE				
				checksum = defIO.checksum;
			}
			else if(dt!=daeDocType::ARCHIVE)
			{
				//HACK! Should be OK regardless
				//of which destructor gets used.
				storedIO dest(*this);
				if(8==values[1]) //CODE 
				new(&dest) deflatedIO(*this); 
				else assert(0==values[1]); //CODE
				if(dt!=0)
				{
					uri.setURI(a.getDocURI(),i.string);
					uri.setIsResolved();
					daeIORequest req(&a,nullptr,&uri,&uri);		
					daeDocRoot<> dr = 
					const_cast<daeDoc*>(i.doc().second);					
					req.fulfillRequestO(in._O,dr,&dest);
					dest.OK = dr.error;
				}
				else dest.OK = write(dest,a,i.string);
				OK = dest.complete();
				values[5] = (int)dest.smallsize; //SMALL
				values[7] = (int)dest.checksize; //LARGE
				checksum = dest.checksum;				
			}
			else //RECURSIVE (ARCHIVE)
			{
				assert(0==values[1]); //CODE 
				//It occurred to me that here the I/O object 
				//could be passed onto a new I/O request. By
				//the time I realized this there was nothing
				//left to do. It's probably better like this.
				daeIO *swap = crc;
				zipIO crc32(*this);
				crc = &crc32;
				OK = zip((const daeArchive*)i.doc().second); 
				crc = swap;				
				values[5] = //SMALL
				values[7] = (int)crc32.checksize; //LARGE
				checksum = crc32.checksum;				
			}
			if(!OK) return OK;

			//"PK\x06\x07" Data Descriptor (magic word optional.)
			assert(checksum!=0||0==(values[5]|values[7])); //SMALL,LARGE
			daeZAE::BigEndian<32>((int&)lh[14]=checksum);
			daeZAE::BigEndian<32>((int&)lh[18]=values[5]); //SMALL
			daeZAE::BigEndian<32>((int&)lh[22]=values[7]); //LARGE
			crc->writeOut(lh+14,12); 

			//Set position+method~size aside to be retrieved by
			//output_central_directory().
			(int&)lh[4] = eocd_wrote2+align8;
			cd.insert(cd.end(),lh+4,lh+26);

			eocd_entered++;
			eocd_wrote2+=align8+header+values[5]+12; //SMALL
		}	
		
		if(eocd_entered>65535) //Paranoia?
		{
			daeEH::Error<<"ZIP64? Exceeded 65535 entries: "<<eocd_entered;
			return DAE_ERROR;
		}
		daeZAE::BigEndian<32>(eocd_entered); //Per disk (32 is BE magic.)
		daeZAE::BigEndian<32>(eocd_wrote2);

		return DAE_OK;
	}
	daeOK output_central_directory(const daeArchive &a, char eocd[22])
	{			
		//HACK: Figure out how many	output_local_entries.
		short eocd_entered = (short&)eocd[8];
		daeZAE::BigEndian<16>(eocd_entered);
		daeZAE_vec::iterator it = cd.end()-eocd_entered*22;

		short &eocd_entered2 = (short&)eocd[10];
		int &eocd_wrote = (int&)eocd[12]; 				   

		for(iter i(*this,a);i.string!=nullptr;i++)
		{
			char cde[46] = "PK\x01\x02";
			daeZAE::BigEndian<32>((int&)cde[4]=63|63<<16); //UTF-8 (6.3)
			daeZAE::BigEndian<16>((short&)cde[8]=0x408); //UTF-8/"PK\x06\x07"(8 is for 7-Zip)
			//HACK: Copy position+method~size from data set aside by 
			//output_local_entries().
			int position = (int&)*it;
			std::copy(it+4,it+22,cde+10);
			it+=22;
			daeZAE::BigEndian<16>((short&)cde[28]=(short)i.extent);			
			daeZAE::BigEndian<32>((int&)cde[42]=position);
			
			#ifdef NDEBUG
			#error Don't forget 8B alignment.		 			
			#endif
			crc->writeOut(cde,46);
			crc->writeOut(i.string,i.extent);
			eocd_entered2++;
			eocd_wrote+=46+i.extent;			
		}	

		daeZAE::BigEndian<16>(eocd_entered2); //Total (1/1 disks.)		
		daeZAE::BigEndian<32>(eocd_wrote);

		return crc->getError();
	}

public: //writeRequest

	Zipper(daeZAEPlugin &in, daeIO &io)
	:in(in),io(io),gz(),defIO(*this),crc(&io),_now()
	{}	
	~Zipper()
	{
		new_tdefl_compressor(nullptr,nullptr,gz); //delete gz; 
	}

	daeOK zip(const daeArchive *a)
	{	
		char eocd[22] = "PK\x05\x06";
		//daeZAE::BigEndian<32>((int&)eocd[4]=1); //0
		size_t pop = cd.size();
		daeOK OK = output_local_entries(*a,eocd);	
		if(OK) OK = output_central_directory(*a,eocd);
		if(OK) OK = crc->writeOut(eocd,sizeof(eocd));
		cd.resize(pop); return OK;
	}
};
static daeError daeZAE_write_lock_error(daeIO *r, daeIO *w)
{
	if(r!=nullptr);
	daeEH::Error<<"Could not lock ZAE/ZIP for write-op.\n"
	"(I/O platform may not implement write-to-temporary+move logic.)";
	return w->getError();
}
daeOK daeZAEPlugin::writeRequest(daeIO &IO, const_daeURIRef &URI)
{
	const daeIORequest &req = getRequest();

	if(URI==nullptr) unimplemented:
	{
		//daeEH::Error << "Something?";
		return DAE_ERR_NOT_IMPLEMENTED; 
	}
	else if(*URI!=*req.localURI)
	{
		//daeEH::Error << "Something?";
		//This plugin is just converting archives to ZAE or ZIP files.
		goto unimplemented;
	}

	const daeArchive *a = 
	URI->getParentObject()->a<daeArchive>(); if(a==nullptr) 
	{
		//Reminder: Could try to write other kinds of docs here.
		goto unimplemented;
	}

	//HACK: The I/O framework doesn't have means to make a temporary
	//URI, write to it, delete an existing URI, and move the new URI
	//to its final resting place.
	//So instead, a read-lock is placed on the URI, and the platform
	//must recognize that the write-op is locked, and so prepare the
	//temporary I/O target, and do some delete+move logic in closeIO.
	//Normally a write-op sets the size of the file to be 0. In this
	//case, that destroys the ZAE's source.	
	#ifdef NDEBUG
	//Reminder: daeDoc::_write2 has the same problem.
	#error This only partly makes sense in the output context :(
	#endif
	daeRAII::CloseIO RAII(req.scope->getIOController());	
	//Might want to add some methodology to daeAtlas to make it easy
	//to implement this logic. "atlas->lockSource()?"
	daeIORequest src; 
	a->getAtlas().getSource(src); daeIOEmpty _; 
	if(src.remoteURI!=nullptr&&req.remoteURI!=nullptr)
	if(src.remoteURI->getURI()==req.remoteURI->getURI())
	{
		daeIO *readlock = RAII.p.openIO(*this,_);
		RAII = readlock; 
		if(00==readlock->getLock()&&DAE_OK!=readlock->getError())
		return daeZAE_write_lock_error(readlock,readlock); 
	}
		
	//This is the write-lock.
	IO.getLock();
	if(DAE_OK!=IO.getError())
	return daeZAE_write_lock_error(RAII.IO,&IO); 

	//RECURSIVE
	daeZAEPlugin::Zipper root(*this,IO); return root.zip(a);
}
daeOK daeZAEPlugin::writeContent(daeIO &IO, const daeContents &content)
{
	#ifdef NDEBUG
	#error Can this be meaningfully implemented?
	#endif
	assert(0); return DAE_ERR_NOT_IMPLEMENTED; 
}

//---.
}//<-'

#endif //__COLLADA_DOM__ZAE

/*C1071*/