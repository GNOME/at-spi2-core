/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2007 IBM Corp.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* collection.c: implements the Collection interface */

#include <config.h>
#include <glib.h>
#include <stdio.h>
#include <bonobo/bonobo-exception.h>
#include <libspi/accessible.h>
#include <libspi/collection.h>
#include <libspi/matchrule.h> 
#include <libspi/stateset.h>
#include <libspi/spi-private.h>

SpiCollection *
spi_collection_interface_new (AtkObject *obj)
{

     SpiCollection *new_collection = g_object_new (SPI_COLLECTION_TYPE, NULL);
     spi_base_construct (SPI_BASE (new_collection), G_OBJECT (obj));

     return  new_collection;

}


static AtkObject *
get_atkobject_from_servant (PortableServer_Servant servant){

     SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  g_return_val_if_fail (object, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (object->gobj), NULL);
  
  return ATK_OBJECT (object->gobj);
}

static SpiCollection *
get_collection_from_servant (PortableServer_Servant servant)
{
     SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));
     
     g_return_val_if_fail (object, NULL);
     g_return_val_if_fail (IS_COLLECTION (object), NULL);

     return SPI_COLLECTION (object);
				
}


static Accessibility_MatchRule 
impl_createMatchRule (PortableServer_Servant servant,
		      const Accessibility_StateSet states,
		      const Accessibility_Collection_MatchType statematchtype,
		      const Accessibility_AttributeSet *attributes,
		      const Accessibility_Collection_MatchType attributematchtype,
		      const Accessibility_RoleSet *roles,
		      const Accessibility_Collection_MatchType rolematchtype,
		      const CORBA_char *interfaces,
		      const Accessibility_Collection_MatchType interfacematchtype,
		      const CORBA_boolean invert,
		      CORBA_Environment *ev){

     Accessibility_MatchRule  retval = NULL;

     SpiMatchrule *matchrule = spi_matchrule_interface_new ();
     MatchRulePrivate *mrp   = get_collection_from_servant (servant)->_mrp;     
     Accessibility_StateSet ss = CORBA_Object_duplicate (states, ev);
     gint i;


     if (mrp != NULL){
       CORBA_free (mrp->attributes);
       CORBA_free (mrp->roles);
       CORBA_free (mrp->interfaces);

       g_free (mrp);
     }
     
     get_collection_from_servant (servant)->_mrp  = g_new (MatchRulePrivate, 1);
     mrp   = get_collection_from_servant (servant)->_mrp;

     /* states */

     mrp->states  = ss;
     mrp->statematchtype = statematchtype;

     /* attributes */

     mrp->attributes = CORBA_sequence_CORBA_string__alloc ();
     mrp->attributes->_maximum = attributes->_maximum;
     mrp->attributes->_length = attributes->_length;
     mrp->attributes->_buffer = CORBA_sequence_CORBA_string_allocbuf (attributes->_length);

     for (i = 0; i < mrp->attributes->_length; i++)
	  mrp->attributes->_buffer [i]= CORBA_string_dup (attributes->_buffer [i]);
    
     CORBA_sequence_set_release (mrp->attributes, TRUE);
     mrp->attributematchtype = attributematchtype;

     /* roles */

     mrp->roles = Accessibility_RoleSet__alloc ();
     mrp->roles->_maximum = roles->_maximum;
     mrp->roles->_length = roles->_length;
     mrp->roles->_buffer = Accessibility_RoleSet_allocbuf (roles->_length);

     for (i = 0; i < roles->_length; i++)
	  mrp->roles->_buffer [i] = roles->_buffer [i];
     
     CORBA_sequence_set_release (mrp->roles, TRUE);
     mrp->rolematchtype = rolematchtype;

     /* interfaces */
     
     mrp->interfaces = CORBA_string_dup (interfaces);
     mrp->interfacematchtype = interfacematchtype;

     mrp->invert = invert;

     retval = CORBA_Object_duplicate (BONOBO_OBJREF (matchrule), ev);

     return retval;

}

static void impl_freeMatchRule (PortableServer_Servant servant,
				Accessibility_MatchRule matchrule,     
				CORBA_Environment *ev){

     SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));
     SpiCollection *spimatchrule;

     MatchRulePrivate *mrp;  

     spimatchrule = SPI_COLLECTION (object);
     mrp = spimatchrule->_mrp;
     
     CORBA_free (mrp->attributes);
     CORBA_free (mrp->roles);
     CORBA_free (mrp->interfaces);

     g_free (mrp);
     spimatchrule->_mrp = NULL;
}



static gboolean
child_interface_p (Accessibility_Accessible child, gchar *repo_id, CORBA_Environment *ev) {
          
     CORBA_Object retval;
     
     retval = Bonobo_Unknown_queryInterface (child, repo_id, ev);
     
     return (retval != CORBA_OBJECT_NIL)? TRUE : FALSE;

}

#define child_collection_p(ch,ev) (child_interface_p (ch,"IDL:Accessibility/Collection:1.0", ev))

static gboolean
match_states_all_p (Accessibility_Accessible child, Accessibility_StateSet set,  CORBA_Environment *ev){
     

     Accessibility_StateSet chs  ;
     Accessibility_StateSeq *seq = Accessibility_StateSet_getStates (set, ev); 
     gint i;

     if (seq->_length == 0 || seq == NULL)
	  return TRUE;

     chs = Accessibility_Accessible_getState (child, ev);
     
     
     for (i = 0; i < seq->_length; i++)
	  if (!Accessibility_StateSet_contains (chs, seq->_buffer [i], ev))
	       return FALSE;
   
     return TRUE;
	  
}

static gboolean
match_states_any_p  (Accessibility_Accessible child, Accessibility_StateSet set,  CORBA_Environment *ev){
     
     Accessibility_StateSet chs; 
     Accessibility_StateSeq *seq = Accessibility_StateSet_getStates (set, ev);
     gint i;

     if (seq->_length == 0 || seq == NULL)
	  return TRUE;

     chs = Accessibility_Accessible_getState (child, ev);

     for (i = 0; i < seq->_length; i++)
   	  if (Accessibility_StateSet_contains (chs, seq->_buffer [i], ev))
	       return TRUE;

     return FALSE;

}

static gboolean
match_states_none_p (Accessibility_Accessible child, Accessibility_StateSet set,  CORBA_Environment *ev){
     
     Accessibility_StateSet chs; 
     Accessibility_StateSeq *seq = Accessibility_StateSet_getStates (set, ev);
     gint i;

     if (seq->_length == 0)
	  return TRUE; 
     chs = Accessibility_Accessible_getState (child, ev);
     
     for (i = 0; i < seq->_length; i++)
	  if (Accessibility_StateSet_contains (chs, seq->_buffer [i], ev))
	       return FALSE;

     return TRUE;

}

static gboolean
match_states_lookup (Accessibility_Accessible child,  MatchRulePrivate *mrp, CORBA_Environment *ev){

     switch (mrp->statematchtype){
     case Accessibility_Collection_MATCH_ALL : 
	  if (match_states_all_p (child, mrp->states, ev))
	       return TRUE;
	  break;
	  
     case  Accessibility_Collection_MATCH_ANY :
	  if (match_states_any_p (child, mrp->states, ev))
	       return TRUE;
	  break;
	  
     case  Accessibility_Collection_MATCH_NONE :
	  if (match_states_none_p (child, mrp->states, ev))
	       return TRUE;
	  break;

      default : break;	  
     }

     return FALSE;    

}

static gboolean
match_roles_all_p (Accessibility_Accessible child, Accessibility_RoleSet *roles,  CORBA_Environment *ev){

   Accessibility_Role role; 

     if (roles->_length > 1) 
	  return FALSE;
     else if (roles->_length == 0 || roles == NULL)
	  return TRUE;

     role  = Accessibility_Accessible_getRole (child, ev);

     if (role  == roles->_buffer [0])
	  return TRUE;
     else 
	  return FALSE;
    
}


static gboolean
match_roles_any_p (Accessibility_Accessible child, Accessibility_RoleSet *roles, CORBA_Environment *ev){

     Accessibility_Role role; 
     int i;

     if (roles->_length == 0 || roles == NULL)
	  return TRUE;

     role =  Accessibility_Accessible_getRole (child, ev);

     for (i = 0; i < roles->_length; i++)
	  if (role  == roles->_buffer [i])
	       return TRUE;

     return FALSE;

}

static gboolean
match_roles_none_p (Accessibility_Accessible child, Accessibility_RoleSet *roles,  CORBA_Environment *ev){

  Accessibility_Role role ; 
     int i;

     if (roles->_length == 0 || roles == NULL)
	  return TRUE;

     role =  Accessibility_Accessible_getRole (child, ev);

     for (i = 0; i < roles->_length; i++)
	  if (role == roles->_buffer [i])
	       return FALSE;

     return TRUE;
     
}


static gboolean
match_roles_lookup (Accessibility_Accessible child,  MatchRulePrivate *mrp, CORBA_Environment *ev){

      switch (mrp->rolematchtype){
	 case Accessibility_Collection_MATCH_ALL : 
	      if (match_roles_all_p (child, mrp->roles, ev))
		   return TRUE;
	      break;

	 case  Accessibility_Collection_MATCH_ANY :
	      if (match_roles_any_p (child, mrp->roles, ev))
		   return TRUE;
	      break;

	 case  Accessibility_Collection_MATCH_NONE :
	      if (match_roles_none_p (child, mrp->roles, ev))
		   return TRUE;
	      break;

      default : break;
 
	 }

      return FALSE;

}


#define split_ifaces(ifaces) (g_strsplit (ifaces, ";", 0))

static gboolean
match_interfaces_all_p (Accessibility_Accessible obj, gchar *interfaces, CORBA_Environment *ev){
     gchar **ifaces; 
     gint i, length; 

     if (interfaces == NULL)
       return TRUE;

     ifaces = split_ifaces (interfaces);
     length = g_strv_length (ifaces);

     for (i = 0; i < length; i++)
       if (!child_interface_p (obj, ifaces [i], ev)){
	    g_free (ifaces);
	       return FALSE;
       }
     return TRUE;
     

}

static gboolean
match_interfaces_any_p (Accessibility_Accessible obj, gchar *interfaces, CORBA_Environment *ev){
     gchar **ifaces; 
     gint i, length; 

     if (interfaces == NULL)
       return TRUE;

     ifaces = split_ifaces (interfaces);
     length = g_strv_length (ifaces);

     for (i = 0; i < length; i++)
       if (child_interface_p (obj, ifaces [i], ev)){
	        g_free (ifaces);
		return TRUE;
       }
     
     return FALSE;
}

static gboolean
match_interfaces_none_p (Accessibility_Accessible obj, gchar *interfaces, CORBA_Environment *ev){

 gchar **ifaces = split_ifaces (interfaces);
     gint i, length = g_strv_length (ifaces);

     if (length == 0)
	  return TRUE;

     for (i = 0; i < length; i++)
	   if (child_interface_p (obj, ifaces [i], ev))
		return FALSE;
     
     return TRUE;
   

}

static gboolean
match_interfaces_lookup (Accessibility_Accessible child, MatchRulePrivate *mrp, CORBA_Environment *ev){

     switch (mrp->interfacematchtype){

     case Accessibility_Collection_MATCH_ALL : 
	  if (match_interfaces_all_p (child, mrp->interfaces, ev))
	       return TRUE;
	  break;
	  
     case  Accessibility_Collection_MATCH_ANY :
	  if (match_interfaces_any_p (child, mrp->interfaces, ev))
	       return TRUE;
	  break;
	  
     case  Accessibility_Collection_MATCH_NONE :
	  if (match_interfaces_none_p (child, mrp->interfaces, ev))
	       return TRUE;
	  break;

      default : break;	  
     }

     return FALSE;     
}

#define split_attributes(attributes) (g_strsplit (attributes, ";", 0))

static gboolean 
match_attributes_all_p (Accessibility_Accessible child, Accessibility_AttributeSet  *attributes, CORBA_Environment *ev){

     int i, k;
     Accessibility_AttributeSet *oa ;
     gboolean flag = FALSE;

     if (attributes->_length == 0 || attributes == NULL)
	  return TRUE;
     
     oa =  Accessibility_Accessible_getAttributes (child, ev);

     for (i = 0; i < attributes->_length; i++){
	  for (k = 0; k < oa->_length; k++)
	       if (!g_ascii_strcasecmp (oa->_buffer [k], attributes->_buffer [i])){
		    flag = TRUE;
                    break;
               }
	       else
		    flag = FALSE;
	  if (!flag) 
	       return FALSE; 
     }
     return TRUE;
}

static gboolean 
match_attributes_any_p (Accessibility_Accessible child, Accessibility_AttributeSet  *attributes, CORBA_Environment *ev){

     int i, k;

     Accessibility_AttributeSet *oa;

     if (attributes->_length == 0 || attributes == NULL)
	  return TRUE;

     oa =  Accessibility_Accessible_getAttributes (child, ev);

     for (i = 0; i < attributes->_length; i++)
	  for (k = 0; k < oa->_length; k++)
	       if (!g_ascii_strcasecmp (oa->_buffer [k], attributes->_buffer[i]))
		    return TRUE;
     return FALSE;
}

static gboolean 
match_attributes_none_p (Accessibility_Accessible child, Accessibility_AttributeSet  *attributes, CORBA_Environment *ev){

     int i, k;
     Accessibility_AttributeSet *oa;
     gboolean flag = FALSE;
     
     if (attributes->_length == 0 || attributes == NULL)
	  return TRUE;

     oa =  Accessibility_Accessible_getAttributes (child, ev);

     for (i = 0; i < attributes->_length; i++){
	  for (k = 0; k < oa->_length; k++)
	       if (!g_ascii_strcasecmp (oa->_buffer [k], attributes->_buffer [i]))
		    return FALSE;
     }
     return TRUE;
}

static gboolean
match_attributes_lookup (Accessibility_Accessible child, MatchRulePrivate *mrp, CORBA_Environment *ev){

     switch (mrp->attributematchtype){

	  case Accessibility_Collection_MATCH_ALL : 
	  if (match_attributes_all_p (child, mrp->attributes, ev))
	       return TRUE;
	  break;
	  
     case  Accessibility_Collection_MATCH_ANY :
	  if (match_attributes_any_p (child, mrp->attributes, ev))
	       return TRUE;
	  break;
	  
     case  Accessibility_Collection_MATCH_NONE :
	  if (match_attributes_none_p (child, mrp->attributes, ev))
	       return TRUE;
	  break;

      default : break;	  
     }

     return FALSE;   



}

static gboolean
traverse_p (Accessibility_Accessible child, 
            const CORBA_boolean traverse,
	    CORBA_Environment *ev){

  if (traverse)
    return TRUE;
  else return !child_collection_p (child, ev);
        
}

static int 
sort_order_canonical (MatchRulePrivate *mrp, GList *ls, 		      
		      gint kount, gint max,
		      Accessibility_Accessible obj, glong index, gboolean flag, 
		      Accessibility_Accessible pobj, CORBA_boolean recurse, 
		      CORBA_boolean traverse, CORBA_Environment *ev){

     gint i = index;
     glong acount  = Accessibility_Accessible__get_childCount (obj, ev);
     gboolean prev = pobj? TRUE : FALSE;
   
     for (; i < acount && (max == 0 || kount < max); i++){
	  Accessibility_Accessible child = Accessibility_Accessible_getChildAtIndex (obj, i, ev);

	          
	  if (prev && CORBA_Object_is_equivalent (child, pobj, ev)){

	    return kount;           

	  }
	 
	  if (flag  && match_interfaces_lookup (child, mrp, ev) && match_states_lookup (child, mrp, ev)     
                    && match_roles_lookup (child, mrp, ev)  
		    && match_attributes_lookup (child, mrp, ev)
	      	    ){
	   
	    ls = g_list_append (ls, child);
	    
	    kount++;
	  }
	    
	  if (!flag)
	       flag = TRUE;

	 
	  if (recurse && traverse_p (child, traverse, ev))
	    kount = sort_order_canonical (mrp, ls,  kount, max, child, 0, TRUE, pobj, recurse, traverse, ev);  
     }
     return kount;
} 

static int
query_exec (MatchRulePrivate *mrp,  Accessibility_Collection_SortOrder sortby, 
	    GList *ls, gint kount, gint max, 
	    Accessibility_Accessible obj, glong index, 
	    gboolean flag, 
	    Accessibility_Accessible pobj,
	    CORBA_boolean recurse, CORBA_boolean traverse,
            CORBA_Environment *ev){
     switch (sortby) {
     case Accessibility_Collection_SORT_ORDER_CANONICAL :  kount = sort_order_canonical  (mrp, ls, 0, max, obj, index, flag, pobj, recurse, traverse, ev); 
       break;
     case Accessibility_Collection_SORT_ORDER_REVERSE_CANONICAL :
       kount = sort_order_canonical  (mrp, ls, 0, max, obj, index, flag, pobj, recurse, traverse, ev);    
	   break;
       
     default: kount = 0; g_warning ("Sort method not implemented yet"); break; 
     }
     
     return kount;

}


static Accessibility_AccessibleSet *
_accessible_list_to_set (GList *ls, gint kount){
    Accessibility_AccessibleSet *retval;
    gint i;
   
     retval = Accessibility_AccessibleSet__alloc ();
     retval->_maximum = kount; 
     retval->_length = kount; 
     retval->_buffer = Accessibility_AccessibleSet_allocbuf (kount);

     for (i=0; i < kount; i++){
       retval->_buffer [i] = ls->data;
       ls = g_list_next (ls);
     }
     
     CORBA_sequence_set_release (retval, TRUE);
     
     return retval;
}

static Accessibility_AccessibleSet *
impl_getMatchesFrom (PortableServer_Servant servant,
		      const Accessibility_Accessible current_object,
		      const Accessibility_MatchRule rule,
		      const Accessibility_Collection_SortOrder sortby,
		      const CORBA_boolean isrestrict,
		      CORBA_long  count,
		      const CORBA_boolean traverse,
		      CORBA_Environment *ev){
    
     GList *ls = NULL;
     Accessibility_Accessible parent; 
     MatchRulePrivate *mrp;
     glong index = Accessibility_Accessible_getIndexInParent (current_object, ev);
     gint kount = 0;

     ls = g_list_append (ls, current_object);
     mrp =  get_collection_from_servant (servant)->_mrp;;
          
     if (!isrestrict){
          parent = Accessibility_Accessible__get_parent (current_object, ev);
	  kount = query_exec (mrp,  sortby, ls, 0, count, parent, index, FALSE, CORBA_OBJECT_NIL, TRUE, traverse, ev);

     }
     else 
	  kount = query_exec (mrp,  sortby, ls, 0,count,  current_object, 0, FALSE, CORBA_OBJECT_NIL, TRUE, traverse, ev);


     ls = g_list_next (ls); 

     if (sortby == Accessibility_Collection_SORT_ORDER_REVERSE_CANONICAL)
       ls = g_list_reverse (ls);
 
     return  _accessible_list_to_set (ls, kount);
}


static Accessibility_AccessibleSet *
impl_getMatchesTo (PortableServer_Servant servant,
		      const Accessibility_Accessible current_object,
		      const Accessibility_MatchRule rule,
		      const Accessibility_Collection_SortOrder sortby,
		      const CORBA_boolean recurse, 
		      CORBA_long  count,
		      const CORBA_boolean traverse,
		      CORBA_Environment *ev){


  GList *ls = NULL;
  AtkObject *aobj;
  Accessibility_Accessible obj;
  MatchRulePrivate *mrp;
   gint kount = 0;


  ls = g_list_append (ls, current_object); 
  mrp =  get_collection_from_servant (servant)->_mrp;
  
  
  if (recurse){
    obj = Accessibility_Accessible__get_parent (current_object, ev);
    kount =  query_exec (mrp,  sortby, ls, 0, count,  obj, 0, TRUE, current_object, TRUE, traverse, ev);
  }
  else{
    aobj = get_atkobject_from_servant (servant);
    obj = spi_accessible_new_return (aobj, FALSE, ev);
    kount = query_exec (mrp,  sortby, ls, 0, count, obj, 0, TRUE, current_object, TRUE, traverse, ev); 

  }

  ls = g_list_next (ls); 
   
  if (sortby == Accessibility_Collection_SORT_ORDER_REVERSE_CANONICAL)
    ls = g_list_reverse (ls);
 
  return  _accessible_list_to_set (ls, kount);
  
}



static Accessibility_AccessibleSet *
impl_getMatches (PortableServer_Servant servant,
		 const Accessibility_MatchRule rule,
		 const Accessibility_Collection_SortOrder sortby,
		 CORBA_long  count,
                 const CORBA_boolean traverse,
		 CORBA_Environment *ev){
     GList *ls = NULL;
     AtkObject *aobj = get_atkobject_from_servant (servant);
     Accessibility_Accessible obj;
     MatchRulePrivate *mrp;
     gint kount = 0;
    
     obj = spi_accessible_new_return (aobj, FALSE, ev);
     ls = g_list_prepend (ls, obj); 
     mrp =  get_collection_from_servant (servant)->_mrp;
     
     kount = query_exec (mrp,  sortby, ls, 0, count, obj, 0, TRUE, CORBA_OBJECT_NIL, TRUE, traverse, ev); 

     ls = g_list_next (ls); 
    
     if (sortby == Accessibility_Collection_SORT_ORDER_REVERSE_CANONICAL)
       ls = g_list_reverse (ls);

     return  _accessible_list_to_set (ls, kount);
}

static void
spi_collection_class_init (SpiCollectionClass *klass)
{

    POA_Accessibility_Collection__epv *epv  = &klass->epv;

    /*    
      epv->isAncestorOf = impl_isAncestorOf; 
    */

    epv->createMatchRule = impl_createMatchRule;
    epv->freeMatchRule   = impl_freeMatchRule;
    epv->getMatches      = impl_getMatches;
    epv->getMatchesTo    = impl_getMatchesTo;
    epv->getMatchesFrom  = impl_getMatchesFrom;
    

    /*
      epv->getActiveDescendant = impl_getActiveDescendant;
    */

}

static void
spi_collection_init (SpiCollection *collection)
{

  //collection->_mrp = g_new (MatchRulePrivate, 1);

}

BONOBO_TYPE_FUNC_FULL (SpiCollection,
		       Accessibility_Collection,
		       SPI_TYPE_BASE,
		       spi_collection)

