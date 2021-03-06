/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_META_CM_POLICY_H__
#define __COLLADA_DOM__DAE_META_CM_POLICY_H__

#include "daeContent.h"

COLLADA_(namespace)
{//-.
//<-'

class daeParentCM;

/**
 * Class @c daeCM is the base-class for the content-model policy classes 
 * which are used to describe the availability and order of	the children.
 * @see The "findChild" APIs are moved to @c daeMetaElement::findChild().
 */
class daeCM
{	
COLLADA_(public) //"vptr" DATA-MEMBER

	#ifdef _DEBUG
	/**
	 * Default Constructor
	 */
	daeCM():_maxOrdinals(),_meta(),_parentCM(){}
	#endif
	
COLLADA_(protected) //Implementation details	
	/**
	 * This is now stored so the placement procedures can be unrolled.
	 */
	daeXS _xs; 

	friend XS::Sequence;
	/**
	 * Historically the library has used "ordinals" to place children.
	 */
	daeCounter _ordSubtrahend, _maxOrdinals, _maxOrdinals_x_maxOccurs;

COLLADA_(protected) //XML Schema relationships	
	/**
	 * The element that this CM graph is modeling.
	 */
	daeMeta *_meta;

	friend daeMetaElement;
	/**WARNING
	 * @warning @c XS::Group spawns elements that retain parents
	 * from their global group. I think that is to retain access 
	 * to said group. In that case, walking up the graph through
	 * the parent enters a different CM-graph altogether.
	 *
	 * Parent XML Schema element in the CM. The root is @c nullptr.
	 * @c daeMetaElement::~daeMetaElement() sets this to @c nullptr
	 * in order to communicate that it owns/destructs @c this object.
	 */
	daeParentCM *_parentCM;		

COLLADA_(protected) //XML Schema values	
	/**WARNING
	 * Minimum number of times this meta element can occur. 
	 *
	 * @warning This is the schema value. The library doesn't likely use it.
	 */
	daeCounter _minOccurs;
	/**WARNING
	 * @warning This is the schema value. Use @c isUnbounded() to determine
	 * if "unbounded." The system limit depends on ordinals. 32-bit ordinal
	 * builds are typically limited to about 3000 occurrences of a sequence,
	 * -choice, or group, if they have a parent in the CM graph. Degenerate 
	 * cases can be excepted in theory, although a review would be in order.
	 *
	 * Maximum number of times this meta element can occur; "-1" for unbounded.
	 */
	daeCounter _maxOccurs;

COLLADA_(public) //OPERATORS
	
	COLLADA_DOM_OBJECT_OPERATORS(daeCM)

COLLADA_(public)
	/**
	 * Gets @c XS::ALL,ANY,CHOICE,ELEMENT,GROUP&SEQUENCE.	 
	 * These correspond to the types of, @c XS::Any, etc.
	 */
	inline daeXS getXS()const{ return _xs; }

	/**
	 * Tells if this CM node is a @c daeParentCM.
	 * @note Parent nodes (@c _parentCM) are @c daeParentCM.
	 */
	inline bool isParentCM()const{ return _xs<0; }
	/**
	 * Tells if this CM node is a @c daeParentCM.
	 * @note Parent nodes (@c _parentCM) are @c daeParentCM.
	 */
	static bool isParentCM(daeXS xs){ return xs<0; }

	/**WARNING
	 * @warning The parents of xs:group included elements is
	 * the global xs:group element's CM-graph.
	 * Tells if @c this node is not the root of its CM graph.
	 */
	inline bool hasParentCM()const{ return _parentCM!=nullptr; }

	/**WARNING
	 * @warning @c XS::Group spawns elements that retain parents
	 * from their global group. I think that is to retain access 
	 * to said group. In that case, walking up the graph through
	 * the parent enters a different CM-graph altogether.
	 *
	 * Gets the parent of this content-model policy object.
	 * @return Returns @c nullptr if @c this is a CM-graph's top.
	 */
	inline daeParentCM *getParentCM(){ return _parentCM; }
	/**WARNING, CONST-PROPOGATING-FORM
	 * @warning @c XS::Group spawns elements that retain parents
	 * from their global group. I think that is to retain access 
	 * to said group. In that case, walking up the graph through
	 * the parent enters a different CM-graph altogether.
	 *
	 * Gets the parent of this content-model policy object.
	 * @return Returns @c nullptr if @c this is a CM-graph's top.
	 */
	inline const daeParentCM *getParentCM()const{ return _parentCM; }

	/**
	 * Gets the containing @c daeMetaElement for this @c daeCM.
	 * @return Returns @c nullptr if the content-model is an empty variety. 
	 */
	inline daeMeta &getMeta()const{ return *_meta; }	

	/**
	 * Gets the containing @c daeMetaElement for this @c daeCM.
	 * @return Returns @c nullptr if the content-model is an empty variety. 
	 */
	inline const XS::Schema &getSchema()const
	{
		return **(DAEP::Make**)_meta; //INCOMPLETE
	}

	/**WARNING
	 * Gets the XML Schema maxOccurs attribute.
	 * This implementation converts @c int into @c unsigned, where -1 is "unbounded."
	 * @warning Don't test this for "unbounded".
	 * @see @c isUnbounded().
	 * @see @c _maxOccurs Doxygentaiton.
	 */
	inline daeCounter getMaxOccurs()const{ return _maxOccurs; }

	/**
	 * Gets the XML Schema minOccurs attribute.
	 */
	inline daeCounter getMinOccurs()const{ return _minOccurs; }

	/**
	 * Tells if maxOccurs is "unbounded."
	 */
	inline bool isUnbounded()const
	{
		//This is mimicking _daeCM_init(). 32-bits is sufficent.
		return _maxOccurs==daeCounter(-1); daeCTC<sizeof(_maxOccurs)==sizeof(daeCounter)>();
	}

	/**
	 * Formerly "__subtrahend."
	 * Cross-access of this comes up often.
	 * It's a public API in case it needs to be related to 
	 * @c XS::Element::hasSubstitutes(). 
	 * @remark The term "subtrahend" isn't related to substitution groups, 
	 * -but it almost sounds like it could be.
	 * (It's subtracted from the parent's ordinal in local ordinal-space.)
	 */
	inline daeCounter getSubtrahend()const
	{
		return _ordSubtrahend; daeCTC<sizeof(_ordSubtrahend)==sizeof(daeOrdinal)>();
	}

	#ifndef COLLADA_NODEPRECATED
	COLLADA_DEPRECATED("Post-2.5: END-OF-SUPPORT\n\
	Use daeMetaElement::findChild(). Its search is not narrowed.")
	//The library never used this limited to the scope of a "daeMetaCMPolicy" based class.
	//It could just call _meta->findChild() but the semantics wouldn't be clearly defined.
	void findChild(const void*)const;
	#endif //COLLADA_NODEPRECATED

COLLADA_(public) //Formerly of XS::Attribute	
	/**
	 * Formerly "isArrayAttribute," although the meaning is
	 * slightly different this time.
	 * @see @c isArrayChildID() for a use-case that is closer
	 * to the old "isArrayAttribute."
	 */
	inline bool canReoccur()const{ return _maxOccurs>1; }

	/**LEGACY-SUPPORT
	 * Previoiusly "getIsRequired."
	 * Tells if the schema indicates that this is a required attribute.
	 * @return Returns true if this is a required attribute, false if not.
	 */
	inline bool isRequired()const{ return _minOccurs>0; }
	
#ifdef BUILDING_COLLADA_DOM

COLLADA_(public) //INVISIBLE
	/**
	 * Pseudo Constructor
	 * @param container The daeMetaElement that this policy object belongs to.
	 * @param parent The daeCM parent of this policy object.
	 * @param odinal The ordinal value offset of this specific policy object. Used for maintaining the 
	 * correct order of child elements.
	 * @param minO The minimum number of times this CMPolicy object must appear. This value comes from the COLLADA schema.
	 * @param maxO The maximum number of times this CMPolicy object may appear. This value comes from the COLLADA schema.
	 * @remarks HISTORICALLY GENERATORS OUTPUT -1 FOR "unbounded." @c isUnbounded() IS COUNTING ON THIS. -1 IS LESS TEXT.
	 */
	inline void _daeCM_init(daeXS xs, daeMetaElement *meta, daeParentCM *parent, daeCounter subtrahend, int minO, int maxO) 
	{
		_xs = xs; _meta = meta; _parentCM = parent; _ordSubtrahend = subtrahend; _minOccurs = minO; _maxOccurs = maxO;
	}

COLLADA_(public) //FORMERLY ABSTRACT INTERFACES
	/**
	 * Tells if an existing choice is compatible with @c this CM node.
	 * @param minuend is an ordinal count from the @c _parentCM frame.
	 */
	inline bool __claim(daeCounter minuend)const
	{
		return minuend-_ordSubtrahend<_maxOrdinals; 
	}
	
	template<class A, class B>
	/**
	 * Tells if @c this CM node intersects ordinals from @a a to @a b.
	 * Does code-elimination for the non-between placement procedures.
	 */
	inline bool __between(A,daeOrdinal,B,daeCounter daeCM::*_=&daeCM::_maxOrdinals)const
	{
		return false; daeCTC<0>(); (void)_;
	}

	friend class daeMetaElement;			
	template<class T, class A, class B, class C>
	/** 
	 * Implements @c _placeElement() etc. 
	 */
	inline daeError __placeT(A,daeCM_Placement<C>&,B,daeOrdinal)const;
	template<int Inner, class A, class B, class C>
	/**
	 * Switches @c _placeElement() etc. on @c _xs. 
	 * @tparam Inner must be 0 for outer placement (i.e. the first placement
	 * into the root CM node) or 1 for inner placements (i.e. any placements
	 * into internal CM nodes.) The 0 and 1 look like an O or I respectively.
	 */
	inline daeError __placeXS(A,daeCM_Placement<C>&,B,daeOrdinal)const;	

	/**
	 * Places an element into the parent element based on this content-model policy object.
	 * @return Returns the @c daeOrdinal via @a p.name_in_ordinal_out.extent.
	 * @todo If a @c daeError code is wanted, it can be returned via "extent" as well.
	 */
	daeError _placeElement(daeCM_Placement<>&)const;
	/** Implements @c daeMetaElement::placeAfter() etc. */
	daeError _placeElementBetween(daeOrdinal,daeCM_Placement<>&,daeOrdinal)const;	
	/**WARNING
	 * This algorithm locates the ordinals around an <xs:choice> which has
	 * had an element removed from it, and attempts to translate the choice
	 * to a "lower-energy" solution that excludes the removed ordinal, while
	 * retaining the remaining ordinals. 
	 *
	 * COLLOQUIALLY if the process of refining a choice is called "promotion,"
	 * -then this can be thought of as a "demotion," or rather an attempt at it.
	 * @c XS::Element's @c namefellows_demote member is 1 if any given name might
	 * trigger a demotion. The caller is presumed to have consulted it.
	 *
	 * @param demotion_information repurposes @c daeCM_Placement<>. All fields
	 * are filled out. @c ordinal holds the removed ordinal. @c insertion_point
	 * holds the removed content pointer. It is used to note surrounding content.
	 * If @c count==0 scanning is not required. The members may differ upon return.
	 *
	 * @warning THIS API IS CRITICAL TO CORRECT OPERATION OF THE LIBRARY. IT MUST
	 * BE INVOLVED IN ANY-&-ALL REMOVAL OPERATIONS THAT CAN'T RULE OUT <xs:choice>.
	 */
	void _chooseWithout(daeCM_Demotion<> &demotion_information)const;
	/**RECURSIVE Implements _chooseWithout(). */
	daeContent *_chooseWithout2(daeCM_Demotion<>&,daeOrdinal)const;

	/**
	 * Implements @c daeMetaElement::addContentModel(). 
	 * @return Returns an optimized entrypoint for @c daeMeta::getCMEntree().
	 */
	daeCM *_prepareContentModel()
	{
		//UNIMPLEMENTED
		#ifdef NDEBUG //GCC and quotes/apostrophes.
		#error "Don't \"return this;\" Many CMs have redundancies that can be skipped."
		#error "For instance, domAny has an <xs:sequence> that\'s strictly pro-forma."
		#endif
		_prepareContentModel2(); return this; //UNIMPLEMENTED
	}
	/**RECURSIVE Implements @c daeMetaElement::addContentModel(). */
	void _prepareContentModel2();

	template<class T>
	/** Implements @c __subsetofXS(). */
	inline bool __subsetofT(const daeCM&,int,int)const;
	/** Used to prioritize choices. */
	inline bool __subsetofXS(const daeCM&)const;

	//SCHEDULED FOR REMOVAL
	/**
	 * Removing virtual destructors, moving toward @c daeStringAllocator.
	 */
	//inline void _self_delete()const;
	
#endif //BUILDING_COLLADA_DOM

COLLADA_(public) //daeParentCM::getCM iterator
	
	//Defining here since daeCM is less 
	//typing than daeParentCM::iterator.
	typedef const daeCM*const *pointer;
	/**
	 * This is a quick/dirty graph walking API.
	 * Simple iterator for use with @c getCM().
	 * @c operator->() alleviates (*p)-> style
	 * notation.
	 */
	struct iterator
	{
		pointer _ptr;
				  
		inline iterator operator=(pointer ptr)
		{
			_ptr = ptr; return *this;
		}

		inline operator pointer&(){ return _ptr; }

		inline const daeCM *operator->(){ return *_ptr; }
	};

	//Using allocator in lieu of destructor.
	typedef std::vector<const daeCM*,daeSA> _CM_vector;
};

/**
 * @c XS::Sequence and @c XS::Choice (and "XS::All"?) are based on @c daeParentCM.
 * @todo: VARIABLE-LENGTH makes sense for @c _deepCM, but it can wait until later.
 */
class daeParentCM : public daeCM
{
COLLADA_(public)
	/**OVERLOAD
	 * This is a quick/dirty graph walking API.
	 */
	inline iterator getCM(size_t &n)const
	{
		iterator b,e; getCM(b,e); n = e-b; return b;
	}
	COLLADA_NOALIAS
	/**OVERLOAD
	 * This is a quick/dirty graph walking API.
	 */
	COLLADA_DOM_LINKAGE void getCM(iterator&b,iterator&e)const
	COLLADA_DOM_SNIPPET( b = &*_CM.begin(); e = &*_CM.end(); )	

#ifdef BUILDING_COLLADA_DOM
	
	//Would be nice if get_allocator returned a reference???
	daeParentCM(daeSA a):_CM(a){} 

	//std::string SHOULD BE TEMPORARY.
	typedef short _CM_index;	
	typedef std::char_traits<_CM_index> _DeepCM_traits;
	typedef std::basic_string<_CM_index,_DeepCM_traits,daeSA> _DeepCM_string;		
	
COLLADA_(protected) //INVISIBLE
	
	friend class XS::Element;
	friend class daeMetaElement;

	/**
	 * This is managed by @c daeMetaElement. 
	 */
	_CM_vector _CM;
	
	/**TODO
	 * This is jump table per the child-ID names.
	 * The elements index into @c _CM.
	 * Hack: for @c XS::Choice there are @c _CM.size()
	 * additional tacked onto the end.
	 * @see @c XS::Choice::_claimCM.
	 *
	 * @todo IT WOULD BE USEFUL if @c _deepCM[1].back()
	 * returned @c _CM.size(), and if "_CM[_CM.size()]"
	 * would find @c getMeta()->jumpIntoTOC(1). That is
	 * so that the unnamed child-ID (1) is valid to use
	 * to make placements so that it doesn't have to be
	 * handled especially prior to making the placement.
	 */
	_DeepCM_string *_deepCM;
	
	friend class XS::Group;
	/**
	 * Currently double entries are not appended, so
	 * to simplify some of the algorithms. Additionally
	 * this formulation takes care of the propogating the
	 * entry to the containing "compositors."
	 *
	 * @param test enforces a standard nesting pattern onto
	 * generators; @c setChild() calls @c _deepCM_push_back().
	 */
	inline void _deepCM_push_back(size_t i, const daeCM *test)
	{
		size_t item = _CM.size()-1; assert(test==_CM[item]);
		size_t j = _deepCM[i].size();
		if(j==0||_deepCM[i][j-1]!=(_CM_index)item)
		_deepCM[i].push_back((_CM_index)item);
		if(_parentCM!=nullptr) _parentCM->_deepCM_push_back(i,this);
	}

friend class daeCM;
COLLADA_(protected) //daeCM methods	

	template<class CM>
	/** 
	 * Used to prioritize choices. 
	 */
	inline bool __subsetof(const CM&,int,int)const;

COLLADA_(public) //INVISIBLE
	/**
	 * Cross-access of this comes up often.
	 */
	inline bool __names(int name)const{ return !_deepCM[name].empty(); }

#endif //BUILDING_COLLADA_DOM
};		

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_META_CM_POLICY_H__
/*C1071*/
