(* 定义自然数 *)
Inductive nat :=
| O : nat (* 0 是自然数 *)
| S : nat -> nat. (* 后继函数 *).

(* 自然数前驱函数 *)
Definition pred (n : nat) :=
  match n with
  | O => O
  | S n' => n'
  end.

(* 自然数加法 *)
Fixpoint plus (n m : nat) : nat :=
  match n with
  | O => m
  | S n' => S (plus n' m)
  end.

(* 自然数乘法 *)
Fixpoint mult (n m : nat) : nat :=
  match n with
  | O => O
  | S n' => plus m (mult n' m)
  end.

(* 定义整数 *)
Inductive Z :=
| pos : nat -> Z (* 正整数 *)
| neg : nat -> Z (* 负整数 *)
| zero : Z.      (* 0 *)

(* 辅助函数：自然数减法 *)
Fixpoint minus (n m : nat) : nat :=
  match n, m with
  | O, _ => O
  | n', O => n'
  | S n', S m' => minus n' m'
  end.

(* 整数减法 *)
Definition subZ (n m : Z) : Z :=
  match n, m with
  | pos n', pos m' => 
      match minus n' m' with
      | O => zero
      | S _ as res => pos res
      end
  | pos n', neg m' => pos (plus n' (S m'))
  | neg n', pos m' => neg (plus n' (S m'))
  | neg n', neg m' =>
      match minus m' n' with
      | O => zero
      | S _ as res => neg res
      end
  | _, zero => n
  | zero, _ => 
      match m with
      | pos m' => neg m'
      | neg m' => pos m'
      | zero => zero
      end
  end.

(* 整数加法 *)
Definition addZ (n m : Z) : Z :=
  match n, m with
  | pos n', pos m' => pos (plus n' m')
  | pos n', neg m' => subZ (pos n') (pos m')
  | neg n', pos m' => subZ (pos m') (pos n')
  | neg n', neg m' => neg (plus n' m')
  | zero, _ => m
  | _, zero => n
  end.

(* 定义有理数 *)
Inductive Q :=
| rat : Z -> Z -> Q. (* 表示分子和分母 *)

(* 辅助函数：整数乘法 *)
Definition mulZ (n m : Z) : Z :=
  match n, m with
  | pos n', pos m' => pos (mult n' m')
  | pos n', neg m' => neg (mult n' m')
  | neg n', pos m' => neg (mult n' m')
  | neg n', neg m' => pos (mult n' m')
  | zero, _ => zero
  | _, zero => zero
  end.

(* 有理数乘法 *)
Definition mulQ (n m : Q) : Q :=
  match n, m with
  | rat n1 d1, rat n2 d2 => rat (mulZ n1 n2) (mulZ d1 d2)
  end.

(* 有理数除法 *)
Definition divQ (n m : Q) : option Q :=
  match n, m with
  | rat n1 d1, rat n2 d2 =>
      match m with
      | rat zero _ => None (* 分母不能为零 *)
      | _ => Some (rat (mulZ n1 d2) (mulZ d1 n2))
      end
  end.

(* 定义柯西序列 *)
Inductive Cauchy_seq (A : Type) (dist : A -> A -> nat -> nat) := 
  | cauchy_seq : (nat -> A) -> (forall (eps : nat), exists N, forall n m, n >= N -> m >= N -> dist (f n) (f m) < eps) -> Cauchy_seq A.

(* 实数的定义 *)
Definition Real := Cauchy_seq nat.

(* 复数定义：复数是实数的有序对 (a, b)，其中 a 是实数部分，b 是虚数部分 *)
Inductive Complex : Type := 
  | complex : Real -> Real -> Complex.

(* 辅助函数：有理数到实数的转换 *)
Definition Q_to_R (q : Q) : R := real q.

(* 实数加法 *)
Definition addR (r1 r2 : R) : R :=
  match r1, r2 with
  | real q1, real q2 => real (addQ q1 q2) (* 使用有理数加法近似 *)
  end.

(* 实数减法 *)
Definition subR (r1 r2 : R) : R :=
  match r1, r2 with
  | real q1, real q2 => real (subQ q1 q2)
  end.

(* 实数乘法 *)
Definition mulR (r1 r2 : R) : R :=
  match r1, r2 with
  | real q1, real q2 => real (mulQ q1 q2)
  end.

(* 实数除法 *)
Definition divR (r1 r2 : R) : option R :=
  match r1, r2 with
  | real q1, real q2 =>
      match divQ q1 q2 with
      | None => None (* 除以 0 情况返回 None *)
      | Some q => Some (real q)
      end
  end.

（*定义阶乘*）
Fixpoint fact (n : nat) : nat :=
  match n with
  | O => 1
  | S p => (S p) * fact p
  end.

(* 定义实数指数的泰勒级数 *)
Fixpoint exp_taylor_cauchy (x : nat) (n : nat) : Real :=
  match n with
  | O => cauchy_seq (fun _ => 1) (* 级数的第0项是1 *)
  | S p => let term := (x ^ n) / (fact n) in
           let prev := exp_taylor_cauchy x p in
           cauchy_seq (fun k => (term + prev k)%nat) (* 累加每一项 *)
  end.

(* 确保柯西序列收敛 *)
Definition is_convergent (f : Real) : Prop :=
  forall eps, exists N, forall n m, n >= N -> m >= N -> dist (f n) (f m) < eps.




(* 复数加法 *)
Definition addC (c1 c2 : C) : C :=
  match c1, c2 with
  | complex r1 i1, complex r2 i2 =>
      complex (addR r1 r2) (addR i1 i2)
  end.

(* 复数减法 *)
Definition subC (c1 c2 : C) : C :=
  match c1, c2 with
  | complex r1 i1, complex r2 i2 =>
      complex (subR r1 r2) (subR i1 i2)
  end.

(* 复数乘法 *)
Definition mulC (c1 c2 : C) : C :=
  match c1, c2 with
  | complex r1 i1, complex r2 i2 =>
      complex
        (subR (mulR r1 r2) (mulR i1 i2)) (* 实部: r1*r2 - i1*i2 *)
        (addR (mulR r1 i2) (mulR i1 r2)) (* 虚部: r1*i2 + i1*r2 *)
  end.

(* 复数除法 *)
Definition divC (c1 c2 : C) : option C :=
  match c1, c2 with
  | complex r1 i1, complex r2 i2 =>
      let denom := addR (mulR r2 r2) (mulR i2 i2) in (* 分母: r2^2 + i2^2 *)
      match denom with
      | real (rat zero _) => None (* 分母为零 *)
      | _ =>
          Some (complex
                  (divR (addR (mulR r1 r2) (mulR i1 i2)) denom) (* 实部 *)
                  (divR (subR (mulR i1 r2) (mulR r1 i2)) denom)) (* 虚部 *)
      end
  end.

(* 复数指数：e^(a + bi) = e^a * (cos(b) + i sin(b)) *)
Definition exp_complex (z : Complex) : Complex :=
  match z with
  | complex a b => 
      let real_part := exp_real a in
      let imag_part := mul_real (cos b) real_part in
      complex real_part imag_part
  end.


(* 复数的模 *)
Definition mod_complex (z : Complex) : Real :=
  match z with
  | complex r i =>
      let r_squared := addR (mulR r r) (mulR i i) in (* 模的平方 r^2 + i^2 *)
      sqrt r_squared (* 模为 r^2 + i^2 的平方根 *)
  end.

(* 复数的幅角（假设可以计算） *)
Definition arg_complex (z : Complex) : Real :=
  match z with
  | complex r i =>
      (* 此处可以使用反正切函数等方法来计算幅角 θ *)
      atan2 i r (* 计算反正切，得出幅角 *)
  end.

(* 复数对数的定义，无需对实数部分做对数 *)
Definition log_complex (z : Complex) : Complex :=
  match z with
  | complex r i =>
      let r_value := mod_complex z in (* 计算模 *)
      let theta_value := arg_complex z in (* 计算幅角 *)
      complex (r_value) theta_value (* 复数对数即为模和幅角的组合，这里直接保留模和幅角 *)
  end.

(* 辅助函数：复数模长 *)
Definition absC (c : C) : R :=
  match c with
  | complex r i =>
      addR (mulR r r) (mulR i i) (* 模长平方 *)
  end.

