# Belish
A programming language similar to human natural language expression  
_A programming language similar to English syntactic structure_

## Naming
Because this is a kind of programming language similar to English syntax, but with some ambiguity removed, the structure is rigorous, and then Berry(My favorite fruit) and English are spliced together, that is, Belish.

## Core idea
Subject predicate object structure.  
What's it meaning?  
### Subject
This is the action initiator, that is, the object that calls a function.  
### Predicate
This represents the action to be performed, that is, the function name.
### Object
This represents the object of an action operation, which is also the parameter of a function.
### Example
```
I say "Hello!";
```
`I` is the subject, `say` is the predicate, `"Hello!"` is the object.  
If we translate this code into JavaScript, it is written as follows:
```
I.say("Hello!");
```

```
my sister say "Hello,", "John!";
```
`my sister` is the subject, `say` is the predicate, `"Hello,"`, `"John!"` is the object.  
If we translate this code into JavaScript, it is written as follows:
```
I.sister.say("Hello,", "John!");
```
### Word type(identifier type)
#### Nouns
Variables or literals
#### Verbs
Functions
#### Pronouns
Used to refer to an object
#### Conjunctions
For logical operation
Such as `and` `or`

## Basic Syntax
### Basic Sentence Structure
\<Subject\> \<Predicate\> \<Objects\>.
### Declare Verb
```
def `verb-name`(`args`) {
    # xxx
}
```
### Declare Class::Verb
```
def `verb-name` -> `class-name`(`args`) {
    # xxx
}
```
### Declare Noun
```
let `noun` = `value`;
```
### Declare Pronoun
```
let `pronoun` -> `value`;# Direct reference
```
```
let `pronoun` => `value`;# Proprietor reference
```
### Literals
Number(/(0[xob])?d+(.d+)?/), String(/".*"/ or /'.*'/), Null(null), Undefined(null), Object(/{.*}/)

#### Operators
=, :, ==, !=, <=, >=, <, >, ( ), [ ], { }, ., (comma), &, |, ^, ~, &&, ||, !, +, -, *, /, %, **, <<, >>, +=, -=, *-, /=, %=, **=, <<=, >>=, &=, |=, ^=, in, of, ~~
#### in/of operator
Format: `<expr> in <expr>`  
Used to detect whether the second expr has a value with the keyword expr1.

#### Operator ~~
Format: n~~m  
Returns all integers in an interval [n, m)  
_note_: If n, m is a floating point number, then the system automatically integrates the value of n, m downwards.

#### Get properties in an object
Use operator. to get properties in an object  
Demo: 
```
let b = a.b;
```
Use operator[] also can get properties  
Demo: 
```
let b = a["b"];
```
#### Prioritization
"[" "." 15  
"(" 14  
"++" "--" "!" "~" "**" 13  
"*" "/" "%" 12  
"+" "-" 11  
"<<" ">>" 10  
">" ">=" "<" "<=" 9  
"==" "!=" 8  
"&" 7  
"^" 6  
"|" 5  
"&&" 4  
"||" "~~" 3  
"=" "*=" "/=" "%=" "+=" "-=" ">>=" "<<=" "^=" "&=" "|=" "in" "of" 2  
"," 1  

### Comments
`// xxx` and `# xxx` are Single-Line Comments.  
`/* xxx */` is the Multiline Comment.
                   
### Imperative Structure
We can omit the subject and refer to this.  
Example:
```
print "Hello world!";
```
