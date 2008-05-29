
#include <atk/atk.h>
#include <string.h>
#include <limits.h>
#include <useful_functions.h>

#include "my-atk-text.h"
//*************************implementation***********************

//Attributes
/*
 * Auxiliary functions create/copy/print/free structures
 * Use the same naming principe, as atk, but without 'atk' prefix
 */
 //AtkAttribute
AtkAttribute* attribute_new(const gchar* name, const gchar* value)
{
    AtkAttribute* attr = g_malloc(sizeof(AtkAttribute));
    if(attr == NULL) return NULL;
    attr->name = g_strdup(name);
    attr->value = g_strdup(value);
    return attr;
}
AtkAttribute* attribute_copy(AtkAttribute* attr)
{
    return attribute_new(attr->name, attr->value);
}
void attribute_print(AtkAttribute* attr)
{
    TRACE("name=%s, value=%s", attr->name, attr->value);
}

//AtkAttributeSet
AtkAttributeSet* attribute_set_copy(AtkAttributeSet* attr)
{
    GSList *tmp;
    AtkAttributeSet* result = g_slist_copy(attr);
    for(tmp = result; tmp != NULL; tmp = tmp->next)
        tmp->data = attribute_copy((AtkAttribute*)tmp->data);
    return result;
}
void attribute_set_print(AtkAttributeSet *set)
{
    if(set == NULL) 
       TRACE0("(empty)");
    else 
       g_slist_foreach(set, (GFunc)attribute_print, NULL);
}

// STATIC FUNCTIONS
//
//auxiliary functions for search tokens
//Number of different characters
#define TABLE_SIZE 256  
//modificator static isn't used because this tables will be use in tests
/*static*/ gboolean  table_word_symbols[TABLE_SIZE],
                     table_sentence_symbols[TABLE_SIZE],
                     table_line_symbols[TABLE_SIZE];
static gboolean *tables[7]={NULL,
    table_word_symbols,
    table_word_symbols,
    table_sentence_symbols,
    table_sentence_symbols,
    table_line_symbols,
    table_line_symbols
    };

static gboolean current_token(const gchar* str, gint offset, gint *token_start, gint *token_end,
    const gboolean table_token_symbols[TABLE_SIZE])
{
    const gchar *current = str + offset;
    if(!table_token_symbols[(guchar)*current])
    {
        return FALSE;
    }
    for( --current; (current >= str) && table_token_symbols[(guchar)*current]; --current);
    *token_start = current - str + 1;
    for(current = str + offset + 1;
        (*current != 0) && table_token_symbols[(guchar)*current]; ++current);
    *token_end = current - str;
    return TRUE;
}
static gboolean next_token(const gchar* str, gint offset, gint *token_start, gint *token_end,
    const gboolean table_token_symbols[TABLE_SIZE])
{
    const gchar *current = str + offset;
    for( ; (*current != 0) && table_token_symbols[(guchar)*current]; ++current);
    if(*current == 0)
        return FALSE;
    for(++current ; (*current != 0) && !table_token_symbols[(guchar)*current]; ++current);
    if(!table_token_symbols[(guchar)*current])
        return FALSE;
    return current_token(str, current - str, token_start, token_end, table_token_symbols);
}
static gboolean previous_token(const gchar* str, gint offset, gint *token_start, gint *token_end,
    const gboolean table_token_symbols[TABLE_SIZE])
{
    const gchar *current = str + offset;
    for( ; (current > str) && table_token_symbols[(guchar)*current]; --current);
    if(current == str)
        return FALSE;
    for( ; (current > str) && !table_token_symbols[(guchar)*current]; --current);
    if(!table_token_symbols[(guchar)*current])
        return FALSE;
    return current_token(str, current - str, token_start, token_end, table_token_symbols);
}


//Range: type of data, containing in list of attributes
typedef struct
{
    gint start,end;//range, containing this attributes
    AtkAttributeSet* attributeSet;
} Range;
//auxiliary functions for ranges
Range* range_new(gint start, gint end)
{
    Range *range = g_malloc(sizeof(Range));
    range->start = start;
    range->end = end;
    range->attributeSet = NULL;
    return range;
}

void range_free(Range* range)
{
    atk_attribute_set_free(range->attributeSet);
    g_free(range);
}
void range_print(const Range*range)
{
    TRACE("[%d,%d):", range->start, range->end);
    attribute_set_print(range->attributeSet);
}
//only for correct list of ranges - ranges shouldn't intersect
gint range_compare(const Range* range1, const Range* range2)
{
    return range1->start - range2->start;//never equal
}
//Bounds of text
void text_bounds_init(TextBounds *bounds)
{
    bounds->base_x = 0;
    bounds->base_y = 0;
    bounds->pixels_above_line = 2;
    bounds->pixels_below_line = 3;
    bounds->size = 8;
    bounds->pixels_between_characters = 1;
    bounds->width = 3;
}

//auxiliary function - create new range according to start_offset and end_offset
AtkTextRange* text_range_new(AtkText* text,
    gint start_offset, gint end_offset, AtkCoordType coord_type)
{
    AtkTextRange* range = g_malloc(sizeof(AtkTextRange));
    if(range == NULL) return NULL;
    range->start_offset = start_offset;
    range->end_offset = end_offset;
    range->content = atk_text_get_text(text, start_offset, end_offset);
    atk_text_get_range_extents(text, start_offset, end_offset, coord_type, &range->bounds);
    return range;
}
// Returns number of line, which contain given character.
// Also return relative offset - offset of this character from start of the line
gint get_character_line(MyAtkText *text, gint offset, gint *relative_offset)
{
    gint result = 0;
    //simple realization - counts lines from start of the text, until reaches offset
    const guchar *text_str = (guchar*)text->str; 
    gboolean state_FSM = table_line_symbols[text_str[0]];
    gint i, last_line_start = 0;
    for(i = 1; i <= offset; state_FSM = table_line_symbols[text_str[i++]])
    {
        if(state_FSM)continue;
        result++;
        last_line_start = i;
    }
    if(relative_offset != NULL) *relative_offset = offset - last_line_start;
    return result;
}
// Compute extent of character,
// as it was at line 'line' and at offset 'relative_offset' in that line
//(geometry) 
void get_extents(MyAtkText *text, gint line, gint relative_offset, AtkTextRectangle *rect)
{
    rect->x = text->bounds.base_x + relative_offset * 
        (text->bounds.width + text->bounds.pixels_between_characters);
    rect->y = text->bounds.base_y + text->bounds.pixels_above_line + line *
        (text->bounds.size + text->bounds.pixels_below_line + text->bounds.pixels_above_line);
    rect->width = text->bounds.width;
    rect->height = text->bounds.size; 
}
//return line, corresponding to given y-coordinate
gint get_point_line(MyAtkText *text, gint y)
{
    //slightly differ from invers operation
    if(y - text->bounds.base_y < 0)return -1;
    return  (y - text->bounds.base_y)
        /(text->bounds.size + text->bounds.pixels_below_line + text->bounds.pixels_above_line);
}
// Returns offset from left boundary of text, correspondind to x-coordinate
gint get_point_relative_offset(MyAtkText *text, gint x)
{
    //slightly differ from invers operation
    if(x - text->bounds.base_x < 0)return -1;
    return (x - text->bounds.base_x) 
        /(text->bounds.width + text->bounds.pixels_between_characters);
}
// Returns offset where given line start(even if this line is empty)
// If line number too small(<0)return -1, if too big - return length of the text 
gint get_offset_at_line_start(MyAtkText *text, gint line)
{
    gint i;
    if(line < 0)return -1;
    if(line == 0)return 0;
    gint len = my_strlen(text->str);
    guchar *str = (guchar*)text->str;
    gint current_line = 0;
    gboolean state_FSM = table_line_symbols[str[0]];
    for(i = 1; i < len; state_FSM = table_line_symbols[str[i++]])
    {
        if(state_FSM || ++current_line != line)continue;
        return i;
    }
    return len;
    
}
// Return offset of character at the given line and at the given offset at this line
// If such character doesn't exist, return -1 
gint get_offset_at_line(MyAtkText *text, gint line, gint relative_offset)
{
    gint j;
    if(line < 0 || relative_offset < 0)return -1; 
    const guchar* str = (guchar*)text->str;
    gint len = my_strlen(text->str);
    gint offset_at_line_start = get_offset_at_line_start(text, line);
    if(offset_at_line_start + relative_offset >= len)return -1;
    for(j = 0; j <= relative_offset; j++) 
        if(!table_line_symbols[str[offset_at_line_start + j]])
            return -1;
    return offset_at_line_start + relative_offset;
}
/*
 * Count ranges of text, which clipping by rel_start_offset and relative_end_offset.
 * 'offset' stands start of search(start of first line),
 * number_of_lines - maximum number of lines for search.
 * If 'ranges' not NULL, writes ranges to it. 'coord_type' used only in this case.
 */
gint count_ranges(MyAtkText *text, gint offset, gint rel_start_offset, gint rel_end_offset, 
                 gint number_of_lines, AtkTextRange** ranges, AtkCoordType coord_type)
{
    guchar *str = (guchar*)text->str;
    gint len = my_strlen(text->str);
    
    gint number_of_ranges = 0;
    gint current_line = 0;
    gint current_line_start = offset;
    for(;(current_line < number_of_lines) && (current_line_start < len); current_line ++)
    {
        if(!table_line_symbols[str[current_line_start]])
        {
            current_line_start++;
            continue;
        }
        gint start_offset,end_offset;
        gchar *tmp_str = atk_text_get_text_at_offset((AtkText*)text, current_line_start,
            ATK_TEXT_BOUNDARY_LINE_END, &start_offset, &end_offset);
        g_free(tmp_str);
        if(end_offset - current_line_start > rel_start_offset)
        {
            if(ranges != NULL)
            {
                gint range_start_offset = current_line_start + rel_start_offset;
                gint range_end_offset =  current_line_start + rel_end_offset + 1;
                if(range_end_offset > end_offset)
                    range_end_offset = end_offset;
                //add element    
                ranges[number_of_ranges] = text_range_new((AtkText*)text, 
                    range_start_offset, range_end_offset, coord_type);
            }
            number_of_ranges++;
        }
        current_line_start = end_offset + 1;
    }
    if(ranges != NULL) ranges[number_of_ranges] = NULL;
    return number_of_ranges;
}

//"free"-functions(for internal using, because them don't emit signals)
void my_atk_text_free_run_attributes(MyAtkText *text)
{
    g_list_foreach(text->attributes, (GFunc)range_free, NULL);
    g_list_free(text->attributes);
    text->attributes = NULL;
}
void my_atk_text_free_default_attributes(MyAtkText *text)
{
    atk_attribute_set_free(text->default_attributes);
    text->default_attributes = NULL;
}
void my_atk_text_clear_selections(MyAtkText *text)
{
    if(text->selections->len != 0)
        g_array_remove_range(text->selections, 0, text->selections->len);
}
void table_symbols_init()
{
    //word
    gint i;
    for(i = TABLE_SIZE - 1;i > 0 ; --i)
        table_word_symbols[i] = g_ascii_isalnum(i);
    table_word_symbols['\0'] = FALSE;
    //sentence
    for(i = TABLE_SIZE - 1;i >= 0x20; --i)
        table_sentence_symbols[i] = TRUE;
    table_sentence_symbols['.'] = FALSE; 
    table_sentence_symbols['!'] = FALSE;
    table_sentence_symbols['?'] = FALSE;
    for(i = 0x1f;i > 0; --i)
        table_sentence_symbols[i] = FALSE;
    table_sentence_symbols['\0'] = FALSE;
    //line
    for(i = TABLE_SIZE - 1;i > 0 ; --i)
        table_line_symbols[i] = TRUE;
    table_line_symbols['\n'] = FALSE;
    table_line_symbols['\0'] = FALSE;
}
void correct_selections_after_insert(MyAtkText *text, gint position, gint length)
{
    gint i;
    GArray* selections = text->selections;
    for(i = selections->len - 1; i >=0; i--)
    {
        TextSelection* sel  = &g_array_index(selections, TextSelection, i);
        if(sel->end_offset >= position) sel->end_offset+= length;
        if(sel->start_offset >= position) sel->start_offset+= length;
        else break;
    }
}
void correct_selections_after_delete(MyAtkText *text, gint position, gint length)
{
    gint i;
    GArray* selections = text->selections;
    for(i = selections->len - 1; i >=0; i--)
    {
        TextSelection* sel  = &g_array_index(selections, TextSelection, i);
        if(sel->start_offset >= position)
        {
             if(sel->start_offset >= position + length)
             {
                sel->start_offset-= length;
                sel->end_offset-= length;
             }
             //position <= sel->start_offset < position + length
             else if(sel->end_offset > position + length)
             {
                sel->start_offset = position;
                sel->end_offset -= length;
             }
             else
             {
                g_array_remove_index(selections, i);
             }
             continue;  
        }
        /*sel->start_offset < position*/
        if(sel->end_offset > position + length) sel->end_offset-= length;
        else if(sel->end_offset > position) sel->end_offset = position;
        break;
    }
}
void correct_attributes_after_insert(MyAtkText* text, gint position, gint length)
{
    GList *attributes = text->attributes;
    GList *tmp;
    //before inserted position
    for(tmp = attributes; tmp != NULL; tmp = tmp -> next)
    {
        Range* range = (Range*)tmp->data;
        if(range->end <= position)continue;
        //range->end > position
        if(range->start < position)
            range->start -= length;//will be restore in the next loop
        break;
    }
    //after inserted position
    for(; tmp != NULL; tmp = tmp -> next)
    {
        Range* range = (Range*)tmp->data;
        range->end += length;
        range->start += length;
    }
}
void correct_attributes_after_delete(MyAtkText* text, gint position, gint length)
{
    GList *attributes = text->attributes;
    GList *tmp = attributes;
    //before deleting range
    for(tmp = attributes; tmp != NULL; tmp = tmp->next)
    {
        Range* range  = (Range*)tmp->data;
        if(range->end <= position) continue;
        if(range->start < position)
        {
             if(range->end > position + length) range->end -= length;
             else range->end = position;
             tmp = tmp->next;
        }
        break;
    }
    //at deleting range
    while(tmp != NULL)
    {
        Range* range = (Range*)tmp->data;
        if(range->start >= position + length) break;
        if(range->end <= position + length)
        {
            GList *tmp1 = tmp->next;
            range_free(range);
            attributes = g_list_remove_link(attributes, tmp);
            tmp = tmp1;
            continue;
        }
        //range->end > position + length
        //range->start < position + length
        range->start = position + length;//will be restored in next loop
        break;
    }
    //after deleting range
    for(;tmp != NULL; tmp = tmp->next)
    {
        Range* range = (Range*)tmp->data;
        range->end -= length;
        range->start -= length;
    }
    text->attributes = attributes;
}
void correct_caret_after_insert(MyAtkText* text, gint position, gint length)
{
    if(text->caret_offset > position)text->caret_offset += length;
}
void correct_caret_after_delete(MyAtkText* text, gint position, gint length)
{
    if(text->caret_offset >= position + length)text->caret_offset -= length;
    else if(text->caret_offset > position) text->caret_offset = position;
}

// Implementation of virtual functions
//******************************my_atk_text_get_character_count*************************
static gint my_atk_text_get_character_count(AtkText *text)
{
    MyAtkText *self = (MyAtkText*)text;
    return my_strlen(self->str);
}
//**************************************my_atk_text_get_text*****************************
static gchar* my_atk_text_get_text(AtkText *text, gint start_offset, gint end_offset)
{
    gchar *str = ((MyAtkText*)text)->str;
    if((start_offset < 0) || (end_offset > my_strlen(str)) || (end_offset <= start_offset))
    {
        //incorrect bounds
        return NULL;
    }
    return g_strndup(str + start_offset, end_offset - start_offset);
    
}
//*******************************my_atk_text_get_character_at_offset************************
static gunichar my_atk_text_get_character_at_offset(AtkText *text, gint offset)
{
    gchar *str = ((MyAtkText*)text)->str;
    if(offset < 0 || offset >= my_strlen(str))
    {
        return 0;
    }
    return (gunichar)str[offset];
}
// In the next 3 functions some code is commented for verify tests themselves on 'mutants'
// in realization.
//******************************my_atk_text_get_text_after_offset***************************
static gchar* my_atk_text_get_text_after_offset(AtkText *text, gint offset,
    AtkTextBoundary boundary_type, gint *start_offset, gint *end_offset)
{
    gchar *str = ((MyAtkText*)text)->str;
    gint len = my_strlen(str);
    if((offset < 0) || (offset >= len))
    {
        return NULL;//incorrect offset
    }
    
    // This variable is set in switch statement. If after that statement variable is TRUE,
    // then return text from 'strat_offset' to 'end_offset'. Otherwise NULL will be returned.
    gboolean is_successed = TRUE;
    
    gint start_tmp;
    gint end_tmp;
    
    switch(boundary_type)
    {
    case ATK_TEXT_BOUNDARY_CHAR:
        if(offset + 1 == len)
        {
            is_successed = FALSE;
            break;
        }
        *start_offset = offset + 1;
        *end_offset = offset + 2;
        is_successed = TRUE;
        break;
    case ATK_TEXT_BOUNDARY_WORD_START:
    case ATK_TEXT_BOUNDARY_SENTENCE_START:
    case ATK_TEXT_BOUNDARY_LINE_START:
        if(!next_token(str, offset, start_offset, &end_tmp, tables[boundary_type]))
        {
            //debug
//            if(current_token(str, offset, start_offset, end_offset, tables[boundary_type]))
//            {
//                is_successed = TRUE;
//                break;
//            }
            is_successed = FALSE;
            break;
        }
        if(!next_token(str, end_tmp, end_offset, &end_tmp, tables[boundary_type]))
        {
            *end_offset = len;
        }
        is_successed = TRUE;
        //debug
//        (*start_offset)++;
//        if(*start_offset > 10) ++(*start_offset);
        break;
    case ATK_TEXT_BOUNDARY_WORD_END:
    case ATK_TEXT_BOUNDARY_SENTENCE_END:
    case ATK_TEXT_BOUNDARY_LINE_END:
        if(!current_token(str, offset, &start_tmp, start_offset, tables[boundary_type]))
        {
            if(!next_token(str, offset, &start_tmp, start_offset, tables[boundary_type]))
            {
                is_successed = FALSE;
                break;
            }
        }
        //debug
//        else if(*start_offset > strlen(str) - 7)
//        {
//           *end_offset = *start_offset + 3;
//            is_successed = TRUE;
//           break;
//        }
        if(!next_token(str, *start_offset, &start_tmp, end_offset, tables[boundary_type]))
        {
            is_successed = FALSE;
            break;
        }
        //debug
//        --(*start_offset);
        is_successed = TRUE;
        
        break;
    default:
        is_successed = FALSE;
    }

    if(is_successed)
    {
        return my_atk_text_get_text(text, *start_offset, *end_offset);
    }
    else
    {
        return NULL;
    }
}
//*******************************my_atk_text_get_text_at_offset*******************************
static gchar* my_atk_text_get_text_at_offset(AtkText *text, gint offset,
    AtkTextBoundary boundary_type, gint *start_offset, gint *end_offset)
{
    gchar *str = ((MyAtkText*)text)->str;
    gint len = my_strlen(str);
    if((offset < 0) || (offset >= len))
    {
        return NULL;
    }
    
    // This variable is set in switch statement. If after that statement variable is TRUE,
    // then return text from 'strat_offset' to 'end_offset'. Otherwise NULL will be returned.
    gboolean is_successed = TRUE;
    
    gint start_tmp;
    gint end_tmp;
    
    switch(boundary_type)
    {
    case ATK_TEXT_BOUNDARY_CHAR:
        *start_offset = offset;
        *end_offset = offset + 1;
        is_successed = TRUE;
        break;
    case ATK_TEXT_BOUNDARY_WORD_START:
    case ATK_TEXT_BOUNDARY_SENTENCE_START:
    case ATK_TEXT_BOUNDARY_LINE_START:
        if(!current_token(str, offset, start_offset, &end_tmp, tables[boundary_type]))
        {
            if(!previous_token(str, offset, start_offset, &end_tmp, tables[boundary_type]))
            {
                is_successed = FALSE;
                break;
            }
        }
        if(!next_token(str, offset, end_offset, &end_tmp, tables[boundary_type]))
        {
            *end_offset = len;
        }
        is_successed = TRUE;
        break;
    case ATK_TEXT_BOUNDARY_WORD_END:
    case ATK_TEXT_BOUNDARY_SENTENCE_END:
    case ATK_TEXT_BOUNDARY_LINE_END:
        if(!current_token(str, offset, &start_tmp, end_offset, tables[boundary_type]))
        {
            if(!next_token(str, offset, &start_tmp, end_offset, tables[boundary_type]))
            {
                is_successed = FALSE;
                break;
            }
        }
        if(!previous_token(str, start_tmp, &start_tmp, start_offset, tables[boundary_type]))
        {
            *start_offset = 0;
        }
        is_successed = TRUE;
        //debug
//        ++(*start_offset);
        break;
    default:
        is_successed = FALSE;
    }

    if(is_successed)
    {
        //debug
//        if(boundary_type == ATK_TEXT_BOUNDARY_LINE_START)
//            return my_atk_text_get_text(text, ++(*start_offset), *end_offset);
        return my_atk_text_get_text(text, *start_offset, *end_offset);
    }
    else
    {
        return NULL;
    }
}
//***********************************my_atk_text_get_text_before_offset******************
static gchar* my_atk_text_get_text_before_offset(AtkText *text, gint offset,
    AtkTextBoundary boundary_type, gint *start_offset, gint *end_offset)
{
    gchar *str = ((MyAtkText*)text)->str;
    gint len = my_strlen(str);
    if((offset < 0) || (offset >= len))
    {
        return NULL;
    }
    
    // This variable is set in switch statement. If after that statement variable is TRUE,
    // then return text from 'strat_offset' to 'end_offset'. Otherwise NULL will be returned.
    gboolean is_successed = TRUE;
    
    gint start_tmp;
    gint end_tmp;
    
    switch(boundary_type)
    {
    case ATK_TEXT_BOUNDARY_CHAR:
        if(offset == 0)
        {
            is_successed = FALSE;
            break;
        }
        *start_offset = offset - 1;
        *end_offset = offset;
        is_successed = TRUE;
        break;
    case ATK_TEXT_BOUNDARY_WORD_START:
    case ATK_TEXT_BOUNDARY_SENTENCE_START:
    case ATK_TEXT_BOUNDARY_LINE_START:
        if(!current_token(str, offset, end_offset, &end_tmp, tables[boundary_type]))
        {
            if(!previous_token(str, offset, end_offset, &end_tmp, tables[boundary_type]))
            {
                is_successed = FALSE;
                break;
            }
        }
        if(!previous_token(str, *end_offset, start_offset, &end_tmp, tables[boundary_type]))
        {
            is_successed = FALSE;
            break;    
        }
        is_successed = TRUE;
        //debug
//        ++(*start_offset);
        break;
    case ATK_TEXT_BOUNDARY_WORD_END:
    case ATK_TEXT_BOUNDARY_SENTENCE_END:
    case ATK_TEXT_BOUNDARY_LINE_END:
        if(!previous_token(str, offset, &start_tmp, end_offset, tables[boundary_type]))
        {
            is_successed = FALSE;
            break;
        }
        if(!previous_token(str, start_tmp, &start_tmp, start_offset, tables[boundary_type]))
        {
            *start_offset = 0;
        }
        is_successed = TRUE;
        break;
    default:
        is_successed = FALSE;
    }

    if(is_successed)
    {
        return my_atk_text_get_text(text, *start_offset, *end_offset);
    }
    else
    {
        return NULL;
    }
}
//*********************************my_atk_text_get_run_attributes*****************
AtkAttributeSet* my_atk_text_get_run_attributes(AtkText* text, gint offset,
    gint *start_offset, gint *end_offset)
{
    GList *tmp;
    GList *attributes = ((MyAtkText*)text)->attributes;
    if(offset < 0 || offset >= my_atk_text_get_character_count(text))
    {
        TRACE0("Incorrect value of offset.");
        return NULL;
    }
    gint start = -1, end = -1;
    for(tmp = attributes; tmp != NULL; tmp = tmp->next)
    {
        Range* range = (Range*)(tmp->data); 
        if(range->end <= offset)
        {
             start = range->end;
             continue;
        }
        if(range->start > offset)
        {
             end = range->start;
             break;
        }
        
        *start_offset = range->start;
        *end_offset = range->end;
        return attribute_set_copy(range->attributeSet);
    }    
    *start_offset = (start == -1) ? 0 : start;
    *end_offset = (end == -1) ? my_atk_text_get_character_count(text) : end;
    return NULL;
}
//*********************************my_atk_text_get_default_attributes*****************
AtkAttributeSet* my_atk_text_get_default_attributes(AtkText* text)
{
    return attribute_set_copy(((MyAtkText*)text)->default_attributes);
}
//*********************************my_atk_text_get_character_extents*****************
void my_atk_text_get_character_extents(AtkText* text, gint offset, gint *x, gint *y,
    gint *width, gint *height, AtkCoordType coord_type)
{
    AtkTextRectangle result;
    gint relative_offset, line;
    line = get_character_line((MyAtkText*)text, offset, &relative_offset);
    get_extents((MyAtkText*)text, line, relative_offset, &result);
    *x = result.x;
    *y = result.y;
    *width = result.width;
    *height = result.height;
}
//*******************************my_atk_text_get_range_extents************************
void my_atk_text_get_range_extents(AtkText *text, gint start_offset, gint end_offset,
    AtkCoordType coord_type, AtkTextRectangle *rect)
{
    //simple - union of extents of the characters, contained in this range
    AtkTextRectangle result, bounds_tmp;
    gint i;

    atk_text_get_character_extents (text, start_offset,
                                  &result.x, &result.y,
                                  &result.width, &result.height,
                                  coord_type);

    for (i = start_offset + 1; i < end_offset; i++)
    {
        my_atk_text_get_character_extents (text, i,&bounds_tmp.x, &bounds_tmp.y, 
            &bounds_tmp.width, &bounds_tmp.height, coord_type);
        
        if(bounds_tmp.x < result.x)
        {
            //corrects left boundary
            result.width += result.x - bounds_tmp.x;
            result.x = bounds_tmp.x; 
        }
        if(bounds_tmp.x + bounds_tmp.width > result.x + result.width)
        {
            //corrects right boundary
            result.width = bounds_tmp.x + bounds_tmp.width - result.x;
        }
        if(bounds_tmp.y < result.y)
        {
            //corrects top boundary
            result.height += result.y - bounds_tmp.y;
            result.y = bounds_tmp.y; 
        }
        if(bounds_tmp.y + bounds_tmp.height > result.y + result.height)
        {
            //corrects buttom boundary
            result.height = bounds_tmp.y + bounds_tmp.height - result.y;
        }
    }
    *rect = result;
}
//**********************************my_atk_text_get_offset_at_point*********************
gint my_atk_text_get_offset_at_point(AtkText* text, gint x, gint y, AtkCoordType coord_type)
{
    gint line, relative_offset;

    line = get_point_line((MyAtkText*)text, y);
    relative_offset = get_point_relative_offset((MyAtkText*)text, x);

    return get_offset_at_line((MyAtkText*)text, line, relative_offset);
}
//*****************************my_atk_text_get_bounded_ranges******************************
AtkTextRange** my_atk_text_get_bounded_ranges(AtkText *text, AtkTextRectangle *rect,
    AtkCoordType coord_type, AtkTextClipType x_clip_type, AtkTextClipType y_clip_type)
{
    MyAtkText *self = (MyAtkText*)text;
    
    gint start_line, end_line, start_rel_offset, end_rel_offset;
    AtkTextRange** result;    
    gint len = my_strlen(self->str);
//macro for simplify return empty ranges when fail to do smth
#define RETURN_EMTPY_RANGES {result = g_malloc(sizeof(AtkTextRange*));result[0] = NULL;return result;}
    //start line
    start_line = (y_clip_type == ATK_TEXT_CLIP_NONE) || (y_clip_type == ATK_TEXT_CLIP_MAX)
        ? 0 : get_point_line(self, rect->y);
    if(start_line < 0) start_line = 0;
    //end line
    end_line = (y_clip_type == ATK_TEXT_CLIP_NONE) || (y_clip_type == ATK_TEXT_CLIP_MIN)
        ? G_MAXINT/2 : get_point_line(self, rect->y  + rect->height);
    if(end_line < 0) RETURN_EMTPY_RANGES;
    //start relative offset
    start_rel_offset = (x_clip_type == ATK_TEXT_CLIP_NONE) || (x_clip_type == ATK_TEXT_CLIP_MAX)
        ? 0 : get_point_relative_offset(self, rect->x);
    if(start_rel_offset < 0) start_rel_offset = 0;
    //end relative offset
    end_rel_offset = (x_clip_type == ATK_TEXT_CLIP_NONE) || (x_clip_type == ATK_TEXT_CLIP_MIN)
        ? G_MAXINT/2 : get_point_relative_offset(self, rect->x + rect->width);
    if(end_rel_offset < 0) RETURN_EMTPY_RANGES;
    //start offset(at the start of 'start_line')
    gint start_offset = get_offset_at_line_start(self, start_line);
    if(start_offset + start_rel_offset >= len) RETURN_EMTPY_RANGES;
 
    //count ranges
    gint number_of_ranges = count_ranges(self, start_offset,
        start_rel_offset, end_rel_offset, end_line - start_line + 1, NULL, coord_type);
    //create array(with just getting size)
    result = g_malloc(sizeof(AtkTextRange*) * (number_of_ranges + 1));
    //write ranges
    count_ranges(self, start_offset,
        start_rel_offset, end_rel_offset, end_line - start_line + 1, result, coord_type);
#undef RETURN_EMPTY_RANGES
    return result;
}

//********************************my_atk_text_get_n_selections*******************************
gint my_atk_text_get_n_selections(AtkText *text)
{
    MyAtkText *self = (MyAtkText*)text;
    return self->selections->len;
}

//********************************my_atk_text_get_selection*******************************
gchar* my_atk_text_get_selection(AtkText *text,
    gint selection_num, gint *start_offset, gint *end_offset)
{
    MyAtkText *self = (MyAtkText*)text;
    GArray *selections = self->selections;
    if(selection_num < 0 || selection_num >= selections->len) return NULL;
    *start_offset = g_array_index(selections, TextSelection, selection_num).start_offset;
    *end_offset = g_array_index(selections, TextSelection, selection_num).end_offset;
    return my_atk_text_get_text(text, *start_offset, *end_offset);
}
//********************************my_atk_text_remove_selection*******************************
gboolean my_atk_text_remove_selection(AtkText *text, gint selection_num)
{
    MyAtkText *self = (MyAtkText*)text;
    GArray *selections = self->selections;
    if(selection_num < 0 || selection_num >= selections->len) return FALSE;
    g_array_remove_index(selections, selection_num);
    
    g_signal_emit_by_name(text, "text-selection-changed");
    return TRUE;
}
//********************************my_atk_text_add_selection*******************************
gboolean my_atk_text_add_selection(AtkText *text, gint start_offset, gint end_offset)
{
    if(start_offset < 0 || end_offset > my_atk_text_get_character_count(text) 
        || start_offset >= end_offset) return FALSE;

    MyAtkText *self = (MyAtkText*)text;
    GArray *selections = self->selections;
    gint i;
    for(i = 0; i < selections->len; i++)
    {
        if(g_array_index(selections, TextSelection, i).start_offset >= start_offset)
        {
            if(g_array_index(selections, TextSelection, i).start_offset < end_offset)
                return FALSE;
            break;
        }
    }    
    TextSelection new_selection;
    new_selection.start_offset = start_offset;
    new_selection.end_offset = end_offset;
    g_array_insert_val(selections, i, new_selection);
    
    g_signal_emit_by_name(text, "text-selection-changed");
    return TRUE;
}
//********************************my_atk_text_set_selection*******************************
gboolean my_atk_text_set_selection(AtkText *text,
    gint selection_num, gint start_offset, gint end_offset)
{
    MyAtkText *self = (MyAtkText*)text;
    GArray *selections = self->selections;
    if(selection_num < 0 || selection_num >= selections->len) return NULL;
    
    if((selection_num == 0 
        || g_array_index(selections, TextSelection, selection_num - 1).end_offset <= start_offset)
        && (selection_num == selections->len - 1
        || g_array_index(selections, TextSelection, selection_num + 1).start_offset >= end_offset)
        )
    {
        //Arrange of selections won't change
        g_array_index(selections, TextSelection, selection_num).start_offset = 
            start_offset;
        g_array_index(selections, TextSelection, selection_num).end_offset =
            end_offset;
        g_signal_emit_by_name(text, "text-selection-changed");
        return TRUE;
    }
    gint start_offset_old = 
        g_array_index(selections, TextSelection, selection_num).start_offset;
    gint end_offset_old = 
        g_array_index(selections, TextSelection, selection_num).end_offset;
    
    my_atk_text_remove_selection(text, selection_num);
    if(!my_atk_text_add_selection(text, start_offset, end_offset))
    {
        //fail when adding selection. Restore initial state.
        my_atk_text_add_selection(text, start_offset_old, end_offset_old);
        return FALSE;
    }
    g_signal_emit_by_name(text, "text-selection-changed");
    return TRUE;

}

//************************************my_atk_text_get_caret_offset******************
gint my_atk_text_get_caret_offset(AtkText *text)
{
    MyAtkText *self = (MyAtkText*)text;
    return self->caret_offset;
}
//************************************my_atk_text_set_caret_offset******************
gboolean my_atk_text_set_caret_offset(AtkText *text, gint offset)
{
    MyAtkText *self = (MyAtkText*)text;
    //caret may be set just after the last character
    if(offset < 0 || offset > my_atk_text_get_character_count(text))return FALSE;
    self->caret_offset = offset;
    g_signal_emit_by_name(self, "text-caret-moved", offset);
    return TRUE;
}

//***********************my_atk_text_insert_text*******************************
void my_atk_text_insert_text(AtkEditableText* text, const gchar* string,
    gint length, gint *position)
{
    gint i;
    MyAtkText* myAtkText = (MyAtkText*)text;
    gchar *str = myAtkText->str;
    gint strlen_old = my_strlen(str);
    
    if(string == NULL) return;
    //correct length
    for(i = 0; i < length; i ++)
    {
        if(string[i] == '\0') {length = i; break;}
    }
    
    if(*position < 0 || *position > strlen_old || length <= 0 )return;
    
    
    gchar *new_str = g_malloc(strlen_old + length + 1);
    if(new_str == NULL)return;
    
    if(*position != 0) 
        memcpy(new_str, str, (size_t)*position);
    memcpy(new_str + *position, string, (size_t)length);
    if(strlen_old != *position) 
        memcpy(new_str + *position + length, str + *position,
            (size_t)(strlen_old - *position));
    new_str[strlen_old + length] = '\0';
    
    g_free(str);
    myAtkText->str = new_str;
    correct_selections_after_insert(myAtkText, *position, length);
    correct_attributes_after_insert(myAtkText, *position, length);
    correct_caret_after_insert(myAtkText, *position, length);
    g_signal_emit_by_name(text, "text-changed::insert", *position, length);
    g_signal_emit_by_name(text, "text-selection-changed");
    g_signal_emit_by_name(text, "text-attributes-changed");
    g_signal_emit_by_name(text, "text-caret-moved", myAtkText->caret_offset);
    
    (*position) += length;
}
//*************************my_atk_text_delete_text*******************
void my_atk_text_delete_text(AtkEditableText* text, gint start_pos, gint end_pos)
{
    MyAtkText* myAtkText = (MyAtkText*)text;
    gchar *str = myAtkText->str;
    gint strlen_old = my_strlen(str);
    
    if(start_pos < 0 || end_pos > strlen_old || start_pos >= end_pos )return;
    if(strlen_old != end_pos)
        memmove(str + start_pos, str + end_pos, strlen_old - end_pos);
    str[start_pos - end_pos + strlen_old] = '\0';
    
    correct_selections_after_delete(myAtkText, start_pos, end_pos - start_pos);
    correct_attributes_after_delete(myAtkText, start_pos, end_pos - start_pos);
    correct_caret_after_delete(myAtkText, start_pos, end_pos - start_pos);
    g_signal_emit_by_name(text, "text-changed::delete", start_pos, end_pos - start_pos);
    g_signal_emit_by_name(text, "text-selection-changed");
    g_signal_emit_by_name(text, "text-attributes-changed");
    g_signal_emit_by_name(text, "text-caret-moved", myAtkText->caret_offset);
}
//***********************my_atk_text_set_text_contents*************************
void my_atk_text_set_text_contents(AtkEditableText* text, const gchar* string)
{
    my_atk_text_delete_text(text, 0, my_atk_text_get_character_count((AtkText*)text));
    gint position = 0;
    my_atk_text_insert_text(text, string, my_strlen(string), &position);
}
//**********************my_atk_text_copy_text***************************
void my_atk_text_copy_text(AtkEditableText* text, gint start_pos, gint end_pos)
{
    MyAtkText* myAtkText = (MyAtkText*)text;
    const gchar *str = myAtkText->str;
    gint strlen_old = my_strlen(str);
    if(start_pos < 0 || end_pos > strlen_old || start_pos >= end_pos )return;
    
    MyAtkTextClass *parent = MY_ATK_TEXT_GET_CLASS(text); 
    g_free(parent->clipboard);
    /*parent->clipboard = g_malloc(end_pos - start_pos + 1);
    
    strncpy(parent->clipboard, str + start_pos, end_pos - start_pos);
    parent->clipboard[end_pos - start_pos] = '\0';*/
    parent->clipboard = g_strndup(str + start_pos, end_pos - start_pos); 
}
//**********************my_atk_text_paste_text***************************
void my_atk_text_paste_text(AtkEditableText *text, gint position)
{
    //NULL-clipboard process corretly
    MyAtkTextClass* parent = MY_ATK_TEXT_GET_CLASS(text);
    my_atk_text_insert_text(text, parent->clipboard, my_strlen(parent->clipboard), &position);
}
//**********************my_atk_text_cut_text***************************
void my_atk_text_cut_text(AtkEditableText* text, gint start_pos, gint end_pos)
{
    my_atk_text_copy_text(text, start_pos, end_pos);
    my_atk_text_delete_text(text, start_pos, end_pos);
}
//*********************my_atk_text_set_run_attributes************************
gboolean my_atk_text_set_run_attributes(AtkEditableText* text, AtkAttributeSet* attrib_set,
    gint start_offset, gint end_offset)
{
    MyAtkText* self = (MyAtkText*)text;
    gint len = atk_text_get_character_count((AtkText*)text);
    if(start_offset < 0 || start_offset >= end_offset || end_offset > len)
        return FALSE;
    GList *attributes = self->attributes;
    GList *tmp = attributes;
    
    while(tmp != NULL)
    {
        Range *range = (Range*)tmp->data;
        if(range->start < start_offset)
        {
            if(range->end <= end_offset)
            {
                if(range->end > start_offset) range->end = start_offset; 
                tmp = tmp->next;
                continue;
            }
            /*range->end > end_offset*/
            Range* additional_range = range_new(end_offset, range->end);
            additional_range->attributeSet = attribute_set_copy(range->attributeSet);
            range->end = start_offset;
            attributes = g_list_insert_before(attributes, tmp->next, additional_range);
            tmp = tmp->next;
            break;
        }
        else/*range->start >= start_offset*/
        {
            if(range->end <= end_offset)
            {
                GList *tmp1 = tmp->next;
                attributes = g_list_remove_link(attributes, tmp);
                tmp = tmp1;
                continue;
            }
            /*range->end > end_offset*/
            if(range->start < end_offset) range->start = end_offset;
            break;
        }
    }
    Range *new_range = range_new(start_offset, end_offset);
    new_range->attributeSet = attribute_set_copy(attrib_set);
    if(tmp == NULL)attributes = g_list_append(attributes, new_range);
    else attributes = g_list_insert_before(attributes, tmp, new_range);
    
    self->attributes = attributes;
    g_signal_emit_by_name(self, "text_attributes_changed");
    return TRUE;
}

//others functions
//sets default attributes
void my_atk_text_set_default_attributes(MyAtkText* text, AtkAttributeSet *set)
{
    atk_attribute_set_free(text->default_attributes);
    text->default_attributes = attribute_set_copy(set);
    g_signal_emit_by_name(text, "text-attributes-changed");
}

void my_atk_text_print_run_attributes(MyAtkText *text)
{
    g_list_foreach(text->attributes, (GFunc)range_print, NULL);
}
void my_atk_text_print_default_attributes(MyAtkText *text)
{
    attribute_set_print(text->default_attributes);
}
//need for separate testing interfaces
void auxiliary_atk_text_set_text_contents(MyAtkText* text, const gchar* string)
{
    my_atk_text_set_text_contents((AtkEditableText*)text, string);
}
void auxiliary_atk_text_set_run_attributes(MyAtkText* text, AtkAttributeSet* attrib_set,
    gint start_offset, gint end_offset)
{
    my_atk_text_set_run_attributes((AtkEditableText*)text, attrib_set, start_offset, end_offset);
}

//initialize/finalize functions
static void my_atk_text_instance_init(GTypeInstance *obj, gpointer g_class)
{
    MyAtkText *self = (MyAtkText*)obj;
    
    self->str = NULL;
    self->attributes = NULL;
    self->default_attributes = NULL;
    text_bounds_init(&self->bounds);
    
    self->selections = g_array_new(FALSE, FALSE, sizeof(TextSelection));
    
    self->caret_offset = 0;
}
static void my_atk_text_instance_finalize(GObject* obj)
{
    MyAtkText *self = (MyAtkText*)obj;
    g_free(self->str);
    my_atk_text_free_run_attributes(self);
    my_atk_text_free_default_attributes(self);
    if(self->selections != NULL)g_array_free(self->selections, FALSE);
}

static void my_atk_text_class_init(gpointer g_class, gpointer class_data)
{
    GObjectClass* g_object_class = (GObjectClass*)g_class;
    //GObject virtual table
    g_object_class->finalize = my_atk_text_instance_finalize;
    //Fills tables of symbols
    table_symbols_init();
    //initialize clipboard
    ((MyAtkTextClass*)g_class)->clipboard = NULL;
}
//Because of static registration of type, finalization function will never been called
//And glib prints warning if use it in registration.
/*static void my_atk_text_class_finalize(gpointer g_class, gpointer class_data)
{
    MyAtkTextClass *self = (MyAtkTextClass*)g_class;
    
    g_free(self->clipboard);
    self->clipboard = NULL; 
}*/
void my_atk_text_interface_init(gpointer g_iface, gpointer iface_data)
{
    AtkTextIface *klass = (AtkTextIface*)g_iface;
    //"get_text"
    klass->get_character_count = my_atk_text_get_character_count;
    klass->get_text = my_atk_text_get_text;
    klass->get_character_at_offset = my_atk_text_get_character_at_offset;
    klass->get_text_after_offset = my_atk_text_get_text_after_offset;
    klass->get_text_at_offset = my_atk_text_get_text_at_offset;
    klass->get_text_before_offset = my_atk_text_get_text_before_offset;
    //"attributes"
    klass->get_run_attributes = my_atk_text_get_run_attributes;
    klass->get_default_attributes = my_atk_text_get_default_attributes;
    //"bounds"
    klass->get_character_extents = my_atk_text_get_character_extents;
    klass->get_range_extents = my_atk_text_get_range_extents;
    klass->get_offset_at_point = my_atk_text_get_offset_at_point;
    klass->get_bounded_ranges = my_atk_text_get_bounded_ranges; 
    //"selection"
    klass->get_n_selections = my_atk_text_get_n_selections;
    klass->get_selection = my_atk_text_get_selection;
    klass->remove_selection = my_atk_text_remove_selection;
    klass->add_selection = my_atk_text_add_selection;
    klass->set_selection = my_atk_text_set_selection;
    //"caret"
    klass->get_caret_offset = my_atk_text_get_caret_offset;
    klass->set_caret_offset = my_atk_text_set_caret_offset;
}

static void my_atk_editable_text_interface_init(gpointer g_iface, gpointer iface_data)
{
    AtkEditableTextIface *klass = (AtkEditableTextIface*)g_iface;
    
    klass->set_text_contents = my_atk_text_set_text_contents;
    klass->set_run_attributes = my_atk_text_set_run_attributes;
    klass->copy_text =  my_atk_text_copy_text;
    klass->insert_text =  my_atk_text_insert_text;
    klass->paste_text = my_atk_text_paste_text;
    klass->cut_text = my_atk_text_cut_text;
    klass->delete_text = my_atk_text_delete_text;
}
GType my_atk_text_get_type()
{
    static GType type = 0;
    if(type == 0)
    {
        static const GTypeInfo typeInfo = 
        {
            sizeof(MyAtkTextClass),
            NULL,                       //base_init
            NULL,                       //base_finalize
            my_atk_text_class_init,     //class_init
            NULL,                       //class_finalize
            NULL,                       //class_data
            sizeof(MyAtkText),
            0,                          //n_preallocs
            my_atk_text_instance_init   //instance_init
        };

        static const GInterfaceInfo AtkTextIface_info = 
        {
            my_atk_text_interface_init,         /* interface_init*/
            NULL,                               /* interface_finalize*/
            NULL                                /* interface_data */
        };
        static const GInterfaceInfo AtkEditableTextIface_info = 
        {
            my_atk_editable_text_interface_init,/* interface_init*/
            NULL,                               /* interface_finalize*/
            NULL                                /* interface_data */
        };
        type = g_type_register_static(MY_TYPE_ATK_OBJECT, "MyAtkText", &typeInfo, 0);
        g_type_add_interface_static(type,
            ATK_TYPE_TEXT,
            &AtkTextIface_info);
        
        g_type_add_interface_static(type,
            ATK_TYPE_EDITABLE_TEXT,
            &AtkEditableTextIface_info);
    }
    return type;    
}
