# Skeletonization

Original            | Skeleton
:------------------:|:------------------:
<img src="./resources/kimia99-images/donkey1.png" height="150"> | <img src="./results/thinning/imgs/skeleton_donkey1.png" height="150">

Skeletonization algorithms implemented in C++

- [x] Thinning
- [ ] Potential Fields

To compile:

```{console}
foo@bar:~/skeletonization/src$ g++ main.cpp `pkg-config --cflags --libs opencv`
foo@bar:~/skeletonization/src$ ./a.out
```
