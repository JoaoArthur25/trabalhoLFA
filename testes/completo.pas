program exemplo;

{ variaveis globais,
  comentario de varias linhas }
var total, i: integer;
media: real;
ok: boolean;

function maior(x: integer; y: integer): integer;
begin
  if x > y then
    Result := x
  else
    Result := y;
end;

procedure calcula(n: integer);
var
  soma: integer;
  k: integer;
begin
  soma := 0;
  k := 1;
  while (k <= n) and not ok do
  begin
    soma := soma + k * 2 - (k div 3);
    k := k + 1
  end;
  total := maior(soma, -1);
  media := total / 2.5;
  ok := (total <> 0) or false
end;

BEGIN
  i := 10;
  calcula(i);   // chamada de procedimento
  if ok then
    if total > 100 then i := 1 else i := 2
END.
