# lambdaexpr

System F-omega interpreter

coc(planned):
k <- ∗ / Πx:A.k / Πa:k.k
A <- a / p / Πx:A.A / ∀a:k.A / Λx:A.A / A [e] / λa:k.A / A A
e <- x                       / Λa:k.e / e [A] / λx:A.e / e e
