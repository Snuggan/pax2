# pax-metrics

List specified metrics.
Given a set of metric and metric set ids, it returns a sorted list of metrics.

## Parameters

### `--metrics`
What metrics to calculate (see [here](metrics-how-to-specify.md)).

### `--json`
Output result + metadata info.

### `--nilsson_level`
For some metrics (see [here](metrics-how-to-specify.md)), ignore *z*-values below this value.


## Example

	pax-metrics --metrics=axtra-allt --nilsson_level=1.85


## See also

- [How to specify metrics.](metrics-how-to-specify.md)
