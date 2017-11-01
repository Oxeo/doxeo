<script type="text/javascript">
$(function () {

    Highcharts.setOptions({
        global: {
            useUTC: false
        }
    });

    var start = moment();
    var end = moment();
    start.subtract(1, 'month');

    $.getJSON('thermostat/temperature_logs.js?start=' + start.format('YYYY-MM-DD%20HH:mm:ss')  + '&end=' + end.format('YYYY-MM-DD%20HH:mm:ss'), function (result) {

        var data = [];
        var previousDate = null;
        var minValue = 50;
        var maxValue = 0;
        
        // Parse data
        $.each(result.records, function(key, val) {
            if (val.temp < 50) {			
                if (previousDate !== null && moment(val.date).diff(previousDate, 'minutes') > 15) {
                    previousDate.add(10, 'minutes');
                    data.push([previousDate.valueOf(), null]);
                }

                previousDate = moment(val.date);
                data.push([previousDate.valueOf(), val.temp]);
				
                if (minValue > val.temp) {
                    minValue = val.temp;
                }

                if (maxValue < val.temp) {
                    maxValue = val.temp;
                }
            }
        });
    
        // Create the chart
        $('#graph_container').highcharts('StockChart', {
            chart : {
                events : {
                    load : function () {
                        var series = this.series[0];
                        var cpt = 0;
                        var y = null;

                        setInterval(function () {
                            if (cpt%30 == 0) {
                                $.getJSON('thermostat/temperature.js', function(result) {
                                    if (result.success) {
                                        y = result.temp;
                                        series.addPoint([(new Date()).getTime(), y], true);
                                    } else {
                                        y = null;
                                    }
                                }).fail(function(jqxhr, textStatus, error) {
                                    alert("Request Failed: " + error);
                                    y = null;
                                });
                            } else if (y != null)  {
                                series.addPoint([(new Date()).getTime(), y], true);
                            }
                            cpt++;
                        }, 1000);
                   }
                }
            },

            rangeSelector : {
                buttons: [{
                    type: 'hour',
                    count: 1,
                    text: '1h'
                }, {
                    type: 'hour',
                    count: 6,
                    text: '6h'
                }, {
                    type: 'hour',
                    count: 12,
                    text: '12h'
                }, {
                    type: 'day',
                    count: 1,
                    text: '1d'
                }, {
                    type: 'week',
                    count: 1,
                    text: '1w'
                }, {
                    type: 'all',
                    text: 'All'
                }],
                inputEnabled: false,
                selected : 2
            },
            
            yAxis: {
                title: {
                    text: 'Temperature (°C)'
                },
                max: maxValue,
                min: minValue
            },
            
            xAxis: {
                ordinal: false,
            },
            
            tooltip: {
                dateTimeLabelFormats: {
                    millisecond: '%A, %B %e, %H:%M',
                    second: '%A, %B %e, %H:%M'
                }
            },
            
            exporting: {
                enabled: false
            },

            series : [{
                name : 'Temperature',
                data : data,
                tooltip: {
                    valueDecimals: 1,
                    valueSuffix: '°C',
                }
            }]
        });
    }).fail(function(jqxhr, textStatus, error) {
        alert("Request Failed: " + error);
    });

});
</script>

<script src="../assets/fullcalendar/moment.min.js" type="text/javascript"></script>
<script src="../assets/highstock/highstock.js"></script>
