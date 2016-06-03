"use strict";

// nice color theme using 256-mode colors
var theme = {};

theme.property  = "0;37";
theme.braces    = "1;30";
theme.sep       = "1;30";

theme.undefined = "1;30";
theme.boolean   = "0;33";
theme.number    = "1;33";
theme.string    = "0;32";
theme.quotes    = "1;32";
theme.escape    = "1;32";
theme.function  = "0;35";
theme.cfunction = "0;35";
theme.thread    = "1;35";

theme.regexp    = "0;31";
theme.date      = "1;34";

theme.null      = "1;34";
theme.object    = "1;34";
theme.buffer    = "1;36";
theme.dbuffer   = "0;36";
theme.pointer   = "0;31";

theme.error     = "1;31";
theme.success   = "1;33;42";
theme.failure   = "1;33;41";
theme.highlight = "1;36;44";

return theme;
