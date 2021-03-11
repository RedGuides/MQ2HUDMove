[MQ2HUD]
Last=Invis
SkipParse=30
CheckINI=100
UpdateInBackground=on
ClassHUD=off
ZoneHUD=off
UseFontSize=on

[Invis]
GroupMember0 =         3,5,900,650,0,234,008,${If[${Me.Invis} , ${Me.Name} Invis,]}
GroupMember1 =      3,5,900,670,0,234,008,${If[${Group.Member[1].Invis} , ${Group.Member[1].Name} Invis,]}
GroupMember2 =      3,5,900,690,0,234,008,${If[${Group.Member[2].Invis} , ${Group.Member[2].Name} Invis,]}
GroupMember3 =      3,5,900,710,0,234,008,${If[${Group.Member[3].Invis} , ${Group.Member[3].Name} Invis,]}
GroupMember4 =      3,5,900,730,0,234,008,${If[${Group.Member[4].Invis} , ${Group.Member[4].Name} Invis,]}
GroupMember5 =      3,5,900,750,0,234,008,${If[${Group.Member[5].Invis} , ${Group.Member[5].Name} Invis,]}


[Dude_server]
Last=dev,invis
SkipParse=30
CheckINI=100
UpdateInBackground=on
ClassHUD=off
ZoneHUD=off
