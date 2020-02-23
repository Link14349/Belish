digit -> `0` | `1` | `2` | `3` | `4` | `5` | `6` | `7` | `8` | `9`
number -> number digit | digit
dop -> `+` | `-` | `*` | `/` | `**` | `%` | `|` `&` | `<<` | `>>`
set -> `=`
compareop -> `<` | `>` | `<=` | `>=` | `==` | `&&` | `||`
preop -> `++` | `--` | `!` | `~`
backop -> `++` | `--`

expr -> number | expr dop expr | expr set expr | expr dop`=` expr | expr compareop expr | preop expr | expr backop | `(`expr`)`
expr -> expr `,` expr
parameter -> expr
parameters -> parameter | parameter `,` parameters | ε
expr -> expr `(` parameters `)`
letOp -> `=` | `->` | `=>`
lets -> identifier letOp expr `,` lets
let -> `let` identifier letOp expr | identifier letOp expr `,` lets
if -> `if` `(` expr `)` block elif
else -> `else` block
elif -> `elif` `(` expr `)` block | `elif` `(` expr `)` block elif | `elif` `(` expr `)` block else | ε
for -> `for` `(` expr `;` expr `;` expr `;` `)` block
while -> `while` `(` expr `)` block
do -> `do` block `while` `(` expr `)`
formalParameter -> identifier | identifier `=` expr
formalParameters -> formalParameters `,` formalParameter | formalParameter | ε
def -> `def` identifier `(` formalParameters `)` block
block -> expr | if | for | while | do | def | let
block -> `{` expr | if | for | while | do | def | let `}`
