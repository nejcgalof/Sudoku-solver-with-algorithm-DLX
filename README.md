# Sudoku solver with algorithm DLX
DLX or dancing links (exact cover structure)

C++ with QT

(STILL SLOVENIAN COMMENT'S - LATER FIX THAT. ANY QUESTION'S, JUST CONTACT ME)
##How to use?
Put numbers in square's: 0 mean empty

##What is sudoku?
Is logic puzzle, usually 9x9 array. In one column, row and square is exactly one number.

![alt tag](http://elmo.sbs.arizona.edu/sandiway/sudoku/challenge2.gif)
##Exact cover
Exact cover describes the problem in the form of a matrix with 1 and 0 values (Boolean or a binary matrix). Each column must at least one number one.
We want get the problem Sudoku Exact cover. Here we do not do anything other than Sudoku rules written in the form of a matrix. The connections between rows and columns uniquely describe the problem.
Each column describes a specific rule. We have 4:
1. Position rule: Only one number can be in a single cell
2. Row rule: only one instance of the number can be in one row
3. Column rule: only one instance of the number can be in one column
4. Box rule:  only one instance of the number can be in the box

Number of colums: ```size * size * 4```, in the case of a 9x9 is 324 columns.
Each column describe rules of Sudoku.
Number of rows: ```size * size * size``` in the case of a 9x9 isu 729 rows.
 Later added another line, called Header.
In our case, we have written in the head rule name and number will be reflected in the columns. We also have extra root node, which indicates the beginning of the header row. 
Introduced the structure, which is interlaced with a double-linked list. With this structure we will simply removing and adding nodes (cover - uncover), which is necessary for the further algorithm.

Example:

![alt tag](https://i.imgur.com/GBxyUgz.png)
![alt tag](https://i.imgur.com/f7z8hSa.png)
##Structure of dancing links algorithm
Knuth's algorithm is designed on a double-linked list, which is formed from exact cover. This structure in a constant time remove (cover) a node:

```
Node.Right.Left = Node.Left;
Node.Left.Right = Node.Right;
```
And add back:

```
Node.Right.Left = Node;
Node.Left.Right = Node;
```
The structure is created by each node is indicated by a pointer to up, down, left and right node.  Header row is all linked. We also have a special node in the head, which is called the root and point to the entire structure. In our case, we find all ones and this nodes connected with another. 

##Algorithm:
Algorithm is simple when we have structure. We need two actions: cover and uncover. Algorithm works on brute-force, and end when we don't have columns in structure. When this happend, we have solution. When we have columns, pick one column and find all rows from this column. We remove this row and all columns connected with this row. This we work recursive. If we don't find solution, we go back to previus version and pick another column. We cover and uncover in constant time. Algorithm is very fast.

##Aplication and measurement:
```
kind of sudoku / number of sudoku / time in MS

Easy / 100 / 6

Easy / 100 / 6

Medium / 100 / 444

Medium / 100 / 436

Hard / 1 / 15

Hard / 1 / 1

Expert / 1 / 10

Expert / 1 / 11

Qassim Hamza / 1 / 129
```

##License
https://www.ocf.berkeley.edu/~jchu/publicportal/sudoku/sudoku.paper.html#DancingLinks

http://lanl.arxiv.org/pdf/cs/0011047

http://garethrees.org/2007/06/10/zendoku-generation/#section-4

http://buzzard.ups.edu/talks/beezer-2010-AIMS-sudoku.pdf

http://uaasoftware.com/blog/demos-tutorials/dancing-links-solving-sudoku/

https://www.kth.se/social/files/54bda0d3f276541354ec0425/HLaestanderMHarrysson_dkand14.pdf

http://www.247sudoku.com/
