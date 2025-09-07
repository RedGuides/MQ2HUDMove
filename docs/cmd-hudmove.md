---
tags:
  - command
---

# /hudmove

## Syntax

<!--cmd-syntax-start-->
```eqcommand
/hudmove <name> <direction> <#> | [ list ] | <name> [delete]
```
<!--cmd-syntax-end-->

## Description

<!--cmd-desc-start-->
Works with the currently loaded HUD. Move or delete elements and sections within MQ2HUD.ini
<!--cmd-desc-end-->

## Options

!!! note "`<name>`"
    Can be the HUD name, [section] name, or `;comment` tag name. Comment tags allow you to define custom sections that can span across multiple ini sections.  
    **Example:** `/hudmove name` (moves a single section)

!!! note "`<direction> <#>`"
    Valid directions: `up`, `down`, `left`, `right`.  
    `#` is number of pixels to move.  
    **Example:** `/hudmove name down 100`

!!! note "`delete`"
    Deletes a single HUD element.

!!! note "`list`"
    Lists all HUDs in the loaded HUD set and all sections in MQ2HUD.ini.

## Examples

If we typed <code>/hudmove ;section1 right 100</code> it would move the HUD elements "ManaRegen" and "BuffCount" 100 units to the right. 

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

Custom sections are defined with the following format:

`;<x> name`
