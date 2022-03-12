# BiGram-EXP

An experiment of Natural Language Processing Course.

Using bigram to find the best way of participle.

The standard of CPP is C++11 or C++17 I don't know, try them if you want to know.

## Usage

### Compile

```
mkdir out
g++ src/init_model.cpp -o out/init
g++ src/main.cpp -o out/main
```

### Build The Model

The *model* is just a text file storing an array and a matrix. I don't know any other better name, just call it *model*.

```
./out/init data/199801-UTF8.txt > data/model
```

### Start Using

```
./out/main data/model
```

### Debug

```
export BIGRAM_DEBUG=1
./out/main
```

## Sample

```
$ ./out/main data/model
loading model...
succeed!
> 他是研究生物化学的
1.0667e-42      他 是 研 究 生 物 化 学 的
4.31417e-39     他 是 研 究 生 物 化学 的
6.02929e-38     他 是 研 究 生 物化 学 的
6.03014e-38     他 是 研 究 生物 化 学 的
2.97204e-37     他 是 研究 生 物 化 学 的
4.87769e-34     他 是 研 究 生物 化学 的
1.20202e-33     他 是 研究 生 物 化学 的
3.40606e-33     他 是 研究生 物 化 学 的
1.67989e-32     他 是 研究 生 物化 学 的
1.68012e-32     他 是 研究 生物 化 学 的
1.37756e-29     他 是 研究生 物 化学 的
1.35903e-28     他 是 研究 生物 化学 的
1.92521e-28     他 是 研究生 物化 学 的
>
```

## Principle

N-gram assume that the possibility of a word appearing in a sentence is only realted with the n-1 words before it. The bigram, is just 2-gram, replacing n with 2.

Image a sentence ABC. In 2-gram the word C appearing possibility is $P(C|B)$, and in 3-gram it's $P(C|AB)$.
We can easily get these priori probability with statistics, assuming the appearing time as its possibility.

$$
P(C|B) = \frac{P(BC)}{P(B)}
$$

$$
P(C|AB) = \frac{P(ABC)}{P(AB)}
$$

For example, in "ABABC".

$$
P(C|B) = \frac{\frac{1}{5}}{\frac{2}{5}}
$$

$$
P(C|AB) = \frac{\frac{1}{4}}{\frac{1}{4}}
$$

For more information, visit [n-gram](https://en.wikipedia.org/wiki/N-gram).

