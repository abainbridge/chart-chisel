# Chart Chisel
A tool for drawing message sequence charts.

For a long time, I've been a user of http://www.mcternan.me.uk/mscgen/. While it is a great program it has a few limitations.

* It hasn't been updated since 2011.
* The PNG output is too low res and doesn't seem to be adjustable.
* The SVG output has different rendering bugs in every SVG renderer I can find. In Chrome, arcs with multi-line text are broken. In Microsoft Word the font kerning is bad.
* There's no way to add a title to the chart.
* It puts arrow labels too close to the arrow. As a result, text longer than the arrow covers the top half of the arrow head.
* I'd like a mode where the chart is displayed, rather than having to open it an an external viewer.
* When displaying the chart, it should continue to monitor the source file for changes. When a change is detected, the chart should be redrawn.
* The syntax of the .msc files is too verbose. MsGenny (https://github.com/sverweij/mscgen_js/blob/master/wikum/msgenny.md) seems like an improvement but lacks support for setting text and background colours. The best features of both should be combined.

## Design decisions

Chart Chisel will only output PNG because the SVG problem is unfixable - different implementations will always likely to produce different rendering results.

As an experiement, the parser will be hand written instead of the Flex and Yacc system that mscgen uses. I suspect Flex and Yacc add more complexity than they remove.
