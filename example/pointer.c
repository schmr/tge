int main (int arg)
{
  int *a, *b, *c, d, e;

  a = &d;
  b = &e;
  d++;
  c = a;
  a = b;
}
