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