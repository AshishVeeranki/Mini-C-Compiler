{
   int a[3], s1, s2;
   s1 = 2;
   a[0] = 1;
   a[1] = 2;
   a[s1] = 3;
   s2 = -(a[2] + s1 * 6) / (a[2] - s1);
   print(s2);
   if s2 > 5 
      then 
          print(s2);
      else {
          int s3;
          s3 = 99;
          s2 = -25;
          print(-s1+s2*s3);
      }
   endif
} 