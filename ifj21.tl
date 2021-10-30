require "ifj21"
function concat (x : string, y : string) : string, integer
local z : integer = 2 * 2
return x .. y, 0 end
function main()
local x : string
local ret: integer
x, ret = concat("ahoj", "svete")
end
main()
