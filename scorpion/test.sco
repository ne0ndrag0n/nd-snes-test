type Vec2
    x as u16
    y as u16
end

function addVectors( first as Vec2, second as Vec2 ) as Vec2
    def result as Vec2 = first

    result.x = result.x + second.x
    result.y = result.y + second.y

    return result
end

def test1 as Vec2 = Vec2( 2, 2 )
def test2 as Vec2 = Vec2( 2, 2 )

def result as Vec2 = addVectors( test1, test2 )