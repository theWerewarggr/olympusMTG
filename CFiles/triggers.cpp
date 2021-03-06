#include ".header_link.h"
#include "../HFiles/10triggers.h"
#include "../HFiles/9modifs.h"

void TriggerEvent::trigger(Player* pl, Target* origin) const {
	for(auto iter = all_triggers.begin(); iter != all_triggers.end(); iter++) (*iter)->trigger(pl, origin);
}

void Trigger::trigger(Player* pl, Target* origin) const {
	pl->addtoPrestack(effects, origin);
}

PlayerPreStackElement::PlayerPreStackElement(PreResolvable *p, Target *org): preRes(p) {
    origin = std::make_unique<Targeter>(org);
}

void CardOracle::get_triggers(const char ttype, TriggerEvent& trigEv) const{
    for(int i=0; i<nb_triggers; i++){
        if(typeof_triggers[i] == ttype){
            trigEv.add_trigger(&(triggers[i]));
            //std::cout << "Trigger of type " << type << "was found and added" << std::endl;
        }
    }
}

/* A BRIEF HISTORY OF TRIGGERS
All triggers: (found after ~30 minutes on the Gatherer, there might be more)
A may be more than one object! (like 'four creatures')

A enters zone B (from C)
A leaves zone B
A attack(s) B
A blocks B
B is blocked
B is not blocked
A becomes (monstrous/renowed/transformed/attached to/enchanted/equipped)
A becomes (tapped/untapped/ phases in/phases out /face up/face down /flipped/unflipped)
A deals C-type damage to B "a BEEBLE deals NONCOMBAT damage to a BRUSHWAGG"
A becomes the target of B //this includes "Psychic Battle"
A-counters are put on B

A gains ability B (cf. Theros Archetypes)
A gains life (because C)
A creates B tokens (because C) "a PINK effect causes YOU to create BEEBLE tokens"
A sacrifices B (because C)
A loses / wins the game
A prevents damage (dealt by B)
A shuffles their library (because C)
A searches their library (because C)
A does special action B, i.e. investigate, explore, clash, scry, crew, regenerate, mutate, on C
A plays a land
A is destroyed (by B) (cf. Karmic Justice)
A is countered (by B) (cf. Lullmage Mentor)
A 's mana pool is emptied of mana B
and last but not least:
you control [no artifacts (cf. covetous dragon), a dwarf (cf. goblins of the flarg)]; this will be the most tricky (perhaps see it as a LTB/ETB trigger?)

A and B can be:
players - permanents - creatures - zones - abilities/effects (aka Resolvables/Targeters?) - damage - cards - spells - enums - bundles - mana
these must have an int descriptor, with all of its relevant effects
There are two types of triggers: self-triggers ("when you cast this spell") and general triggers
Let's implement self-triggers first. For that, trigger-side detection is appropriate

HOW TO IMPLEMENT THIS:
so basically, there are two approaches: trigger-side and triggered-side detection;
1. when a creature dies, the program searches among all permanents for DEATH triggers
2. Blood Artist gives, on ETB, a DEATH trigger with a pointer to itself, to all creatures.

1. there is a "marketplace" where every event is declared and all triggers check whether it's relevant for them
every event must thus have an unique identifier, coupled with its arguments's identifiers
2. This would need all creatures to look for all triggers, so all triggers must be implemented somehow
But on the other side, all possible triggering actions must be implemented somewhere right?
We need a class Event, of which 19 instances will be created
(or perhaps they will be created only when a triggered ability actually enters the game?)
then ANY function within the game will be augmented with a "trigger(int a, int b, int c);"
*/