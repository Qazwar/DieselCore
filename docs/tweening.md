# Tweening

## How to use it

```float interpolate(TweeningType type, float start, float end, float t, float duration);```

You can also get the tweening type by the index.
```TweeningType get_by_index(int index);```
Example:
```TweeningType t = tweening::get_by_index(5);```

## Scripting

You can also use tweening in scripts.
Here is an example:
```R1 = 1.0;
R2 = 4.0;
R3 = TWN(0,R1,R1,TIMER,1.0);```
    
## Supported types

* linear
* easeSinus
* easeInQuad
* easeOutQuad
* easeInOutQuad
* easeInBack
* easeOutBack
* easeInOutBack
* easeInCubic
* easeOutCubic
* easeInOutCubic
* easeInElastic
* easeOutElastic
* easeInOutElastic
* easeInBounce
* easeOutBounce
* easeInOutBounce