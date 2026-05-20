.intel_syntax noprefix

.global vec3_new
vec3_new:
  pxor xmm0, xmm0
  unpcklps xmm1, xmm2
  unpcklps xmm3, xmm0
  movlhps xmm1, xmm3
  movups [rcx], xmm1
  mov rax, rcx
  ret

.global vec3_expand
vec3_expand:
  shufps xmm1, xmm1, 0
  movups [rcx], xmm1
  mov rax, rcx
  ret

.global vec3_add
vec3_add:
  movups xmm0, [rdx]
  addps xmm0, [r8]
  movups [rcx], xmm0
  mov rax, rcx
  ret

.global vec3_sub
vec3_sub:
  movups xmm0, [rdx]
  subps xmm0, [r8]
  movups [rcx], xmm0
  mov rax, rcx
  ret

.global vec3_mul
vec3_mul:
  movups xmm0, [rdx]
  shufps xmm2, xmm2, 0
  mulps xmm0, xmm2
  movups [rcx], xmm0
  mov rax, rcx
  ret

.global vec3_div
vec3_div:
  movups xmm0, [rdx]
  shufps xmm2, xmm2, 0
  divps xmm0, xmm2
  movups [rcx], xmm0
  mov rax, rcx
  ret

.global vec3_piecewise
vec3_piecewise:
  movups xmm0, [rdx]
  mulps xmm0, [r8]
  movups [rcx], xmm0
  mov rax, rcx
  ret

.global vec3_dot
vec3_dot:
  movups xmm1, [rcx]
  mulps xmm1, [rdx]
  movhlps xmm0, xmm1
  addss xmm0, xmm1
  shufps xmm1, xmm1, 1
  addss xmm0, xmm1
  ret

.global vec3_mag
vec3_mag:
  sub rsp, 40
  movss xmm0, [rcx]
  movss xmm1, 4[rcx]
  mov 32[rsp], rbx
  mov ebx, 8[rcx]
  call hypotf
  movd xmm1, ebx
  mov rbx, 32[rsp]
  add rsp, 40
  jmp hypotf

.global vec3_normal
vec3_normal:
  sub rsp, 48
  mov 32[rsp], rbx
  mov 40[rsp], rsi
  movss xmm0, [rdx]
  movss xmm1, 4[rdx]
  mov rbx, rdx
  mov rsi, rcx
  call hypotf
  movss xmm1, 8[rbx]
  call hypotf
  pxor xmm1, xmm1
  ucomiss xmm0, xmm1
  jz vec3_normal.zero
  shufps xmm0, xmm0, 0
  movups xmm1, [rbx]
  divps xmm1, xmm0
vec3_normal.zero:
  movups [rsi], xmm1
  mov rax, rsi
  mov rbx, 32[rsp]
  mov rsi, 40[rsp]
  add rsp, 48
  ret

.section .rodata, "a"
ps.1:
  .float 1, 1, 1, 1
