"use strict";

// nice color theme using 256-mode colors
var theme = {};

theme.property  = "38;5;253";
theme.braces    = "38;5;247";
theme.sep       = "38;5;240";

theme.undefined = "38;5;244";
theme.boolean   = "38;5;220"; // yellow-orange
theme.number    = "38;5;202"; // orange
theme.string    = "38;5;34";  // darker green
theme.quotes    = "38;5;40";  // green
theme.escape    = "38;5;46";  // bright green
theme.function  = "38;5;129"; // purple
theme.cfunction = "38;5;161"; // purple-red
theme.thread    = "38;5;199"; // pink

theme.regexp    = "38;5;214"; // yellow-orange
theme.date      = "38;5;153"; // blue-purple

theme.null      = "38;5;27";  // dark blue
theme.object    = "38;5;27";  // blue
theme.buffer    = "38;5;39";  // blue2
theme.dbuffer   = "38;5;69";  // teal
theme.pointer   = "38;5;124"; // red

theme.error     = "38;5;196"; // bright red
theme.success   = "38;5;120;48;5;22";  // bright green
theme.failure   = "38;5;215;48;5;52";  // bright green
theme.highlight = "38;5;45;48;5;236";  // bright teal with grey background

return theme;
