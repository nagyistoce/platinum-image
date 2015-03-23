# Purpose #
transfer\_mapcolor displays integer images that depict a number of classes, with consecutive integer numbers depicting several classes of voxels e.g. tissue types. Images of this kind are typically stored in an [image\_label](image_label.md) object - transfer\_mapcolor is the default transfer function for [image\_label](image_label.md).

_For more information about labeled images, see [image\_label](image_label.md)._

# Mapping #
The mapping is done as follows:
| **Value** | **Color** |
|:----------|:----------|
| 0 | black, signifies void in the image |
| 1 | white |
| 2 | red |
| 3 | blue |
| 4 | green |
| 5 | magenta |
| 6 | yellow |
| 7 | cyan |

| 8-13 | darker variations |
|:-----|:------------------|
| 14-19 | lighter variations |
| > 19, < 1 | gray |

This color cycle, sans black and white, is then repeated with 1. darker variations 2. lighter variations. This yields 19 distinct colors + black, although light-yellow and light-cyan will be hard to distinguish and thus less usable. Values outside the scale are displayed as gray. The color scale, with numbers, is displayed in the [transfer function](TransferFunctions.md) control area.

# Design rationale #
The color scale is designed to yield maximum contrast for 2 (binary), 4 and 16-value images while allowing scaling between these spans without added complexity. The spectrum is deliberately intermixed to increase contrast between adjacent values to maximize separation when fewer values are used, and to reduce the impact of dyschromatopsia.
White as the first color makes the color scheme consistent with binary images.
The color scale will be the same for all <= 20 value images, which makes it useful to memorize, leading to faster and more reliable comprehension of these images.