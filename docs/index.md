---
tags:
  - plugin
resource_link: "https://www.redguides.com/community/resources/mq2hudmove.134/"
support_link: "https://www.redguides.com/community/threads/mq2hudmove.66838/"
repository: "https://github.com/RedGuides/MQ2HUDMove"
config: "MQ2HUD.ini"
authors: "ieatacid, brainiac, ChatWithThisName"
tagline: "Allows you to move HUDs or sections of HUDs easily"
---

# MQ2HUDMove

<!--desc-start-->
Allows you to move HUDs or sections of HUDs easily.
<!--desc-end-->
### Features

- HUDs can be moved to the location of the cursor, or up/down/left/right by the specified number of units.
- Individual elements can be moved, or user-specified sections.
- HUD colors can also be changed to a number of preset colors.


## Commands

<a href="cmd-hudcolor/">
{% 
  include-markdown "projects/mq2hudmove/cmd-hudcolor.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2hudmove/cmd-hudcolor.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2hudmove/cmd-hudcolor.md') }}

<a href="cmd-hudlist/">
{% 
  include-markdown "projects/mq2hudmove/cmd-hudlist.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2hudmove/cmd-hudlist.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2hudmove/cmd-hudlist.md') }}

<a href="cmd-hudmove/">
{% 
  include-markdown "projects/mq2hudmove/cmd-hudmove.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2hudmove/cmd-hudmove.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2hudmove/cmd-hudmove.md') }}

## Settings

HUDMove doesn't have its own settings, it just modified MQ2HUD.ini settings.

For example, if we typed `/hudmove ;section1 right 100`, it would move the HUD elements "ManaRegen" and "BuffCount" 100 units to the right.

```ini
[elements]
Level=3,2,339,649,0,255,255,${If[${Target.ID},${Target.Level} - ${Target.Class} - ${Target.Distance} - ${Target.LineOfSight},]}
Raid=3,2,1263,468,0,255,255,${If[${Raid.Members}>0,In Raid - ${Raid.Members},]}
TarID=3,2,172,337,0,255,0,ID: ${Target.ID}
;<x> section1
ManaRegen=3,2,272,272,0,255,0,MR: ${Me.ManaRegen}
BuffCount=3,2,272,367,0,255,0,Buffs: ${Me.CountBuffs}
;<x> section2
ER=3,2,172,342,0,255,0,ER:  ${Me.AltAbilityTimer[Eldritch Rune].TimeHMS}
MGB=3,2,172,357,0,255,0,MGB:  ${Me.AltAbilityTimer[Mass Group Buff].TimeHMS}
```
