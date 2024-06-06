Inductive nat :=
| O : nat  (* 0是自然数 *)
| S : nat -> nat.  (* 后继函数 *)

Definition pred (n:nat) :=
  match n with
  | O => O
  | S n' => n'
  end.

Fixpoint plus (n m : nat) : nat :=
  match n with
  | O => m
  | S n' => S (plus n' m)
  end.

Fixpoint mult (n m : nat) : nat :=
  match n with
  | O => O
  | S n' => plus m (mult n' m)
  end.

  Inductive Z :=
| pos : nat -> Z
| neg : nat -> Z
| zero : Z.

Definition subZ (n m : Z) : Z :=
  match n, m with
  | pos n', pos m' => Z.of_nat (minus n' m')
  | pos n', neg m' => pos (S (plus n' m'))
  | neg n', pos m' => neg (S (plus n' m'))
  | neg n', neg m' => Z.of_nat (minus m' n')
  | _, zero => n
  | zero, _ => Z.neg m
  end.

Definition addZ (n m : Z) : Z :=
  match n with
  | pos n' => match m with
              | pos m' => pos (plus n' m')
              | neg m' => subZ (pos n') (pos m')
              | zero => pos n'
              end
  | neg n' => match m with
              | pos m' => subZ (pos m') (pos n')
              | neg m' => neg (plus n' m')
              | zero => neg n'
              end
  | zero => m
  end.

Inductive Q :=
| rat : Z -> Z -> Q.

Definition divQ (n m : Q) : Q :=
  match n, m with
  | rat n' d', rat m' e' => rat (mulZ n' e') (mulZ d' m')
  end.

Definition mulQ (n m : Q) : Q :=
  match n, m with
  | rat n' d', rat m' e' => rat (mulZ n' m') (mulZ d' e')
  end.