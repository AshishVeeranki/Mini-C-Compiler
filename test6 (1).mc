{
   int s3[3], s1, s2;
   s1 = 2;
   s3[0] = 1;
   s3[1] = 2;
   s3[s1] = 3;
   s2 = -(s3[2] + s1 * 6) / (s3[2] - s1);
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
   endif;
   s3[0] = 100;
} 