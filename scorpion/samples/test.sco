// Complete example of Scorpion v0.0.1a
// Function arguments are implicitly byref unless marked "byval"

// system namespace is imported by default
// e.g. system.print == print
// function print( value as string )

type Vec2
    x as u16
    y as u16

    function operatorEquals( other as Vec2 ) as bool
        return \
            x == other.x \
            and y == other.y
    end
end

function addVectors( first as Vec2, second as Vec2 ) as Vec2
    def result as Vec2 = first

    result.x = result.x + second.x
    result.y = result.y + second.y

    return result
end

def test1 as Vec2 = ( 2, 2 )
def test2 as Vec2 = ( 2, 2 )

def result1 as Vec2 = addVectors( test1, test2 )
def result2 as Vec2 = addVectors( test1, test2 )

def results[ 2 ] as Vec2 = ( ( 4, 4 ), ( 4, 4 ) )

for i = 1 to results.length - 1
    if results[ i ] == result1 then
        print( "result1 pass" )
    end

    if results[ i ] == result2 then
        print( "result2 pass" )
    end
end