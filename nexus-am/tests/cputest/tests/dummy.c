int main() {
   __asm__ __volatile__("lidt (0x7e00)");
  return 0;
}
