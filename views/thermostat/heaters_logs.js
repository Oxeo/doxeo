<script type="text/javascript">
$(function () {

    Highcharts.setOptions({
        global: {
            useUTC: true
        }
    });

    var start = moment();
    var end = moment();
    start.subtract(1, 'month');

    $.getJSON('heaters_logs.js?start=' + start.format('YYYY-MM-DD%20HH:mm:ss')  + '&end=' + end.format('YYYY-MM-DD%20HH:mm:ss'), function (result) {

        var data = [];
        var date = null;
        
        // Parse data
        $.each(result.records, function(key, val) {
            date = moment(val.start_date);
            data.push([date.valueOf(), val.duration*1000]);
        });
    
        // Create the chart
        $('#container').highcharts('StockChart', {
            chart : {
                alignTicks: false
            },

            title : {
                text : 'Heating Time'
            },

            yAxis: {
                title: {
                    text: 'Hours'
                },
                type: 'datetime',
                dateTimeLabelFormats: { //force all formats to be hour:minute:second
                    second: '%H:%M',
                    minute: '%H:%M',
                    hour: '%H:%M',
                    day: '%H:%M',
                    week: '%H:%M',
                    month: '%H:%M',
                    year: '%H:%M'
                }
            },
            
            rangeSelector : {
                buttons: [{
                    type: 'week',
                    count: 1,
                    text: '1w'
                }, {
                    type: 'month',
                    count: 1,
                    text: '1m'
                }, {
                    type: 'month',
                    count: 3,
                    text: '3m'
                }, {
                    type: 'all',
                    text: 'All'
                }],
                inputEnabled: false,
                selected : 1
            },
            
            tooltip: {
                formatter: function() {
                    return Highcharts.dateFormat('%d %B', new Date(this.x)) + '<br />Heat: <b>' + 
                            Highcharts.dateFormat('%Hh%M', new Date(this.y)) + '</b>';
                }
            },

            series : [{
                type: 'column',
                name : 'Heat',
                data : data,
                color: '#eb6d00',
                dataGrouping: {
                    forced: true,
                    units: [[
                        'day',
                        [1]
                    ], [
                        'week',
                        [1]
                    ], [
                        'month',
                        [1, 2, 3, 4, 6]
                    ]]
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
