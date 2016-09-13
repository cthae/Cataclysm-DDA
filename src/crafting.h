#ifndef CRAFTING_H
#define CRAFTING_H

#include "item.h"         // item
#include "cursesdef.h"    // WINDOW
#include "requirements.h"

#include <string>
#include <vector>
#include <map>
#include <list>

class recipe_dictionary;
class JsonObject;
class Skill;
using skill_id = string_id<Skill>;
class inventory;
class player;
class npc;

enum body_part : int; // From bodypart.h
typedef int nc_color; // From color.h

using itype_id     = std::string; // From itype.h
using requirement_id = string_id<requirement_data>;

struct recipe {
    private:
        std::string ident_;

        friend class recipe_dictionary;

    public:
        itype_id result = "null";
        int time = 0; // in movement points (100 per turn)
        int difficulty = 0;
        bool valid_to_learn = true;

        /** Fetch combined requirement data (inline and via "using" syntax) */
        const requirement_data& requirements() const {
            return requirements_;
        }

        /** Combined requirements cached when recipe finalized */
        requirement_data requirements_;

        /** Second field is the multiplier */
        std::vector<std::pair<requirement_id, int>> reqs;

        std::map<itype_id,int> byproducts;

        std::string cat;
        // Does the item spawn contained in container?
        bool contained = false;
        // What does the item spawn contained in? Unset ("null") means default container.
        itype_id container = "null";
        std::string subcat;
        skill_id skill_used = skill_id::NULL_ID;
        std::map<skill_id, int> required_skills;
        bool reversible = false; // can the item be disassembled?

        /** set learning requirements equal to required skills at finalization? */
        bool autolearn = false;

        std::map<skill_id, int> autolearn_requirements; // Skill levels required to autolearn
        std::map<skill_id, int> learn_by_disassembly; // Skill levels required to learn by disassembly

        /** If set (zero or positive) set charges of output result for items counted by charges */
        int charges = -1;

        // maximum achievable time reduction, as percentage of the original time.
        // if zero then the recipe has no batch crafting time reduction.
        double batch_rscale = 0.0;
        int batch_rsize = 0; // minimum batch size to needed to reach batch_rscale
        int result_mult = 1; // used by certain batch recipes that create more than one stack of the result

        std::map<itype_id,int> booksets;
        std::set<std::string> flags;

        const std::string &ident() const;

        //Create a string list to describe the skill requirements fir this recipe
        // Format: skill_name(amount), skill_name(amount)
        std::string required_skills_string() const;

        recipe();

        // Create an item instance as if the recipe was just finished,
        // Contain charges multiplier
        item create_result() const;
        std::vector<item> create_results( int batch = 1 ) const;

        // Create byproduct instances as if the recipe was just finished
        std::vector<item> create_byproducts( int batch = 1 ) const;

        bool has_byproducts() const;

        bool can_make_with_inventory( const inventory &crafting_inv, int batch = 1 ) const;
        bool can_make_with_inventory( const inventory &crafting_inv,
                                      const std::vector<npc *> &helpers,
                                      int batch = 1 ) const;
        bool check_eligible_containers_for_crafting( int batch = 1 ) const;

        // Can this recipe be memorized?
        bool valid_learn() const;

        int print_items( WINDOW *w, int ypos, int xpos, nc_color col, int batch = 1 ) const;

        int print_time( WINDOW *w, int ypos, int xpos, int width, nc_color col,
                        int batch = 1 ) const;

        int batch_time( int batch = 1 ) const;

        bool has_flag( const std::string &flag_name ) const;

};

// removes any (removable) ammo from the item and stores it in the
// players inventory.
void remove_ammo( item *dis_item, player &p );
// same as above but for each item in the list
void remove_ammo( std::list<item> &dis_items, player &p );

const recipe *recipe_by_name( const std::string &name );
const recipe *get_disassemble_recipe( const itype_id &type );

// Show the "really disassemble?" query along with a list of possible results.
// Returns false if the player answered no to the query.
bool query_dissamble( const item &dis_item );
const recipe *select_crafting_recipe( int &batch_size );
void pick_recipes( const inventory &crafting_inv,
                   const std::vector<npc *> &helpers,
                   std::vector<const recipe *> &current,
                   std::vector<bool> &available, std::string tab,
                   std::string subtab, std::string filter );
void batch_recipes( const inventory &crafting_inv,
                    const std::vector<npc *> &helpers,
                    std::vector<const recipe *> &current,
                    std::vector<bool> &available, const recipe *r );

void set_item_spoilage( item &newit, float used_age_tally, int used_age_count );
void set_item_food( item &newit );
void set_item_inventory( item &newit );
void finalize_crafted_item( item &newit, float used_age_tally, int used_age_count );

#endif
