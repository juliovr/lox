# Grammar

```
expression     → equality | (",") expression ;
ternary        → expression ("?") expression (":") expression ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
                 | primary ;
primary        → NUMBER | STRING | "true" | "false" | "nil"
                 | "(" expression ")" ;
```