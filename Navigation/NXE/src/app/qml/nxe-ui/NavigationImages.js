var turnLeft = 0
var turnRight = 1
var keepStraight = 2
var turnAround = 3
var turnLeft2 = 4
var turnRight2 = 5
var arrived = 6

var manuverImages = ["images/nav_left_1_bk.png", // 0
                     "images/nav_right_1_bk.png", // 1
                     "images/nav_straight_bk.png", //2
                     "images/nav_turnaround_left_bk.png", //3
                     "images/nav_left_2_bk.png", // 4
                     "images/nav_right_2_bk.png", // 5
                     "images/flag_icon_black.png"// 6
        ]

function textToImage(text) {
    var turnLeftRegexp = new RegExp("Turn.*left.*")
    var turnRightRegexp = new RegExp("Turn.*right.*")
    var followRegexp = new RegExp("Follow the road.*")
    var turnAroundRegexp = new RegExp(".*turn around.*")

    var turnLeft2Regexp = new RegExp("Take the second.*left.*");
    var turnRight2Regexp = new RegExp("Take the second.*right.*");
    var reachDestSoonRegexp = new RegExp("You.*destination.*in")
    var arrivedRegexp = new RegExp("You have arrived")

    if(text.match(turnLeftRegexp)) {
        return manuverImages[turnLeft];
    } else if(text.match(turnRightRegexp)) {
        return manuverImages[turnRight]
    } else if(text.match(followRegexp)) {
        return manuverImages[keepStraight]
    } else if(text.match(turnAroundRegexp)) {
        return manuverImages[turnAround]
    } else if(text.match(turnLeft2Regexp)) {
        return manuverImages[turnLeft2]
    } else if(text.match(turnRight2Regexp)) {
        return manuverImages[turnRight2]
    } else if(text.match(arrivedRegexp) || text.match(reachDestSoonRegexp)) {
        return manuverImages[arrived]
    } else {
        console.debug('oups what to do with ', text)
    }
}
