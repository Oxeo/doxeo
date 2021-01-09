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

        var sData = [];
        var cptId = 0;
        var tabId = [];
        
        // Parse data
        $.each(result.records, function(key, val) {
            if (val.temp < 50) {
                var id;
                
                if (!(val.id in tabId)) {
                    tabId[val.id] = cptId;
                    cptId++;
                    id = tabId[val.id];

                    sData[id] = {};
                    sData[id].name = val.id,
                    sData[id].data = [],
                    sData[id].tooltip = {
                        valueDecimals: 1,
                        valueSuffix: '°C',
                    };
                    sData[id].previousDate = null;
                }
                
                id = tabId[val.id];
            
                if (sData[id].previousDate !== null && moment(val.date).diff(sData[id].previousDate, 'minutes') > 15) {
                    sData[id].previousDate.add(10, 'minutes');
                    sData[id].data.push([sData[id].previousDate.valueOf(), null]);
                }

                sData[id].previousDate = moment(val.date);
                sData[id].data.push([sData[id].previousDate.valueOf(), val.temp]);
            }
        });
    
        // Create the chart
        $('#graph_container').highcharts('StockChart', {
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
                inputEnabled: true,
                selected : 2
            },
            
            yAxis: {
                title: {
                    text: 'Temperature (°C)'
                }
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

            series : sData,
        });
        
        for (i=0; i<sData.length; i++) {
            addButton(i, sData[i].name, sData[i].name);
        }
    }).fail(function(jqxhr, textStatus, error) {
        alert("Request Failed: " + error);
    });

});

function addButton(index, id, name) {
	var buttonEl = document.createElement("button");
	buttonEl.className = 'btn-filter btn-active';
    buttonEl.innerText = name;
    buttonEl.addEventListener('click', e => {
        var series = Highcharts.charts[0].series[index];
        if (series.visible) {
            e.target.className = 'btn-filter';
            series.hide();
        } else {
            e.target.className = 'btn-filter btn-active';
            series.show();
        }
    });
    
	document.getElementById("buttons-panel").appendChild(buttonEl);
}

</script>

<script src="../assets/fullcalendar/moment.min.js" type="text/javascript"></script>
<script src="../assets/highstock/highstock.js"></script>
