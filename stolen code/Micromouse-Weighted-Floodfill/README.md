## Demo

I've uploaded a short [![demo video](https://img.youtube.com/vi/YOUTUBE_VIDEO_ID_HERE/0.jpg)](https://www.youtube.com/watch?v=JVsRV0mu0jg) to YouTube with a turn score of 5, tile score of 10, and a streak score of 0. Notice that near the beginning of the maze, the mouse opts to choose the slightly longer yellow path with less turns rather than the shorter green path with more turns.

<img src="https://github.com/Matthew-Chandler/Micromouse-Weighted-Floodfill/assets/48606413/d102270b-bdf4-4a9a-8d4b-3fe2e14c06f5" width="500">

## Understanding and changing weights

To change the weights, change constants `TURN_SCORE`, `TILE_SCORE`, `STREAK_SCORE`, and `STREAK_MULTIPLIER` in **constants.h**. 

`TURN_SCORE` refers to the penalty for making a turn. \
`TILE_SCORE` refers to the penalty for moving to an adjacent square.\
`STREAK_SCORE` refers to the penalty for moving to each additional consecutive square in a straight line. Since we typically want to reward the mouse moving in one diretion, `STREAK_SCORE` should be set to a negative number to act as a bonus. \
`STREAK_MULTIPLIER` refers to the ramping penalty of moving another consecutive square in a streak. Each additional square in a streak will increase this penalty until the streak is broken. Like `STREAK_SCORE`, this should be negative.

For example, if `TILE_SCORE` is set to 10, `STREAK_SCORE` is set to -1, and `STREAK_MULTIPLIER` is set to -1, moving two squares in one direction is worth (10\*2)-1=19, moving three squares in one direction is worth (10\*3)-1-2=27, moving four squares in one direction is worth (10\*4)-1-2-3=34, and so on. If n is the number of squares moved in one direction, the general formula becomes `(TILE_SCORE * n) + [STREAK_MULTIPLIER * (n - 2)(n - 1) / 2)] + (STREAK_SCORE * n)`. 

If `STREAK_MULTIPLIER` is instead 0, moving two squares in one direction is worth (10\*2)-1=19, moving three squares in one direction is worth (10\*3)-1-1=28, moving four squares in one direction is worth (10\*4)-1-1-1=37, and so on.



