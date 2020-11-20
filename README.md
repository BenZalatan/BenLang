# BenLang
A simple compiler for a very simple custom programming language. As per the usual, this is simply a proof of concept rather than a finished "product". This is also a little on the older side, so forgive the large amount of inefficiencies and downright horrible coding practices.

## Information
"BenLang" (I really couldn't come up with a better name...?) has a few simple commands.

#### VAR
**var (name)** - Create an int variable of value 0.<br/>
**var (name) : (type)** - Create a variable of specified type (type can be **INT**, **FLOAT**, or **STRING**).<br/>
**var (name) = (exp)** - Create a variable with assumed type from an expression.

#### EVAL
**eval (exp)** - Prints the output of an expression.

#### INPUT
**input (name)** - Gets user input and sets variable with specified name to the inputted value.

#### DEL
**del (name)** - Deletes variable with specified name.

#### DUMP
**dump** - Prints all variable information.

#### CLEAR
**clear** - Clears the screen.

#### No Command
**(name) : (type)** - Convert variable with name to a specified type (type can be **INT**, **FLOAT**, or **STRING**).<br/>
**(name) = (exp)** - Set a variable to the value of a given expression.

## Examples
A simple addition calculator:
```
var a
var b
input a
input b
var c = @a + @b
eval @c
```
What am I?
```
var insult
input insult
eval "I am a " + @insult
```
Convert int to string:
```
var a = 50
var b = "% chance of rain"
a : STRING
eval @a + @b
