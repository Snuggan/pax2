# How to specify metrics


A metric identifier has two parts: a *statistic* and a *filter*: `*statistic*\_*filter*`.

## Statistics

- **`count`**		Number of values.
- **`mean`**		Mean of the values.
- **`mean2`**		Mean of the squared values.
- **`variance`**	Sample variance.
- **`skewness`**	Sample skewness.
- **`kurtosis`**	Sample kurtosis.
- **`L2`**		L2-moment (L-scale).
- **`L3`**		L3-moment (L-skewness).
- **`L4`**		L4-moment (L-kurtosis).
- **`mad`**		Median absolute deviation.
- **`p{*n*}`**	Percentile {}, where {} is in \[0, 100\].

You can calculate most other common statistics from the above. 


## Filter

The filter format is either of:

- `all[_geMINcm][_ltMAXcm]`: all points within the optional limits.
- `1ret[_geMINcm][_ltMAXcm]`: first returns within the optional limits.

The stuff between `[...]` is optional.


## Preconfigured sets

There are also a few named sets of metrics, more or less general. 
These may be subject to change, but you may always check them by running `pax-metrics --help`. 
Below `999cm` symbolises the given value of the `--nilsson_level` parameter. 

### `basic-linear`
`variance_all_ge*999cm*`, `p30_all_ge*999cm*`, `p80_all_ge*999cm*`, `p95_all_ge*999cm*`, `count_1ret_ge*999cm*`, `count_1ret`

### `inka-berries` 
`L3_all`, `p30_all`, `count_1ret_ge*999cm*`, `count_1ret`, `mean2_1ret`.

### `extra-allt`
`count_all`, `mean_all`, `mean2_all`, `variance_all`, `skewness_all`, `kurtosis_all`, `L2_all`, `L3_all`, `L4_all`, `mad_all`, `p10_all`, `p20_all`, `p30_all`, `p40_all`, `p50_all`, `p60_all`, `p70_all`, `p80_all`, `p90_all`, `p95_all`, `count_all_ge500cm`, `count_all_ge*999cm*`, `mean_all_ge*999cm*`, `mean2_all_ge*999cm*`, `variance_all_ge*999cm*`, `skewness_all_ge*999cm*`, `kurtosis_all_ge*999cm*`, `L2_all_ge*999cm*`, `L3_all_ge*999cm*`, `L4_all_ge*999cm*`, `mad_all_ge*999cm*`, `p10_all_ge*999cm*`, `p20_all_ge*999cm*`, `p30_all_ge*999cm*`, `p40_all_ge*999cm*`, `p50_all_ge*999cm*`, `p60_all_ge*999cm*`, `p70_all_ge*999cm*`, `p80_all_ge*999cm*`, `p90_all_ge*999cm*`, `p95_all_ge*999cm*`, `count_all_ge1000cm`, `count_all_ge1500cm`, `count_1ret`, `mean_1ret`, `mean2_1ret`, `variance_1ret`, `skewness_1ret`, `kurtosis_1ret`, `L2_1ret`, `L3_1ret`, `L4_1ret`, `mad_1ret`, `p10_1ret`, `p20_1ret`, `p30_1ret`, `p40_1ret`, `p50_1ret`, `p60_1ret`, `p70_1ret`, `p80_1ret`, `p90_1ret`, `p95_1ret`, `count_1ret_ge500cm`, `count_1ret_ge*999cm*`, `mean_1ret_ge*999cm*`, `mean2_1ret_ge*999cm*`, `variance_1ret_ge*999cm*`, `skewness_1ret_ge*999cm*`, `kurtosis_1ret_ge*999cm*`, `L2_1ret_ge*999cm*`, `L3_1ret_ge*999cm*`, `L4_1ret_ge*999cm*`, `mad_1ret_ge*999cm*`, `p10_1ret_ge*999cm*`, `p20_1ret_ge*999cm*`, `p30_1ret_ge*999cm*`, `p40_1ret_ge*999cm*`, `p50_1ret_ge*999cm*`, `p60_1ret_ge*999cm*`, `p70_1ret_ge*999cm*`, `p80_1ret_ge*999cm*`, `p90_1ret_ge*999cm*`, `p95_1ret_ge*999cm*`, `count_1ret_ge1000cm`, `count_1ret_ge1500cm`.


## Examples

- `count_all`
- `extra-allt`
- `count_1ret_lt180cm`
- `count_all_ge500cm_lt1000cm`
- `p95_1ret_ge180cm`
