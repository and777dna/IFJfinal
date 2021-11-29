-- Program 1: Vypocet faktorialu (iterativne)
require "ifj21"

function main() -- uzivatelska funkce bez parametru
  local a : integer
  local vysl : integer = 0
  write("Zadejte cislo pro vypocet faktorialu\n")
  a = readi()
  if a == nil then
      write("a je nil\n") return
  else
  end
  while a < 0 do
    write("Faktorial nelze spocitat\n")
    vysl = 1
    if a > 0 then
      vysl = vysl * a a = a - 1  -- dva prikazy
    else 
    end
    write("Vysledek je: ", vysl, "\n")
  end
end

main()  -- prikaz hlavniho tela programu