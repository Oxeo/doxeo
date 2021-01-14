$(function () {

    Highcharts.setOptions({
        global: {
            useUTC: false
        }
    });

    $.getJSON('/sensor/msg_activities.js', function (result) {
        var sData = [];

        // Parse data
        $.each(result.data, function (key, val) {
            sData.push([moment(val.date).valueOf(), val.number]);
        });

        // Create the chart
        $('#graph_container').highcharts('StockChart', {
            chart: {
                type: 'column'
            },
            rangeSelector: {
                enabled: false,
                inputEnabled: false
            },
            legend: {
                enabled: false
            },
            tooltip: {
                pointFormat: '<b>{point.y} messages</b>'
            },

            series: [{
                name: 'Messages',
                data: sData,
                dataLabels: {
                    enabled: true,
                    rotation: -90,
                    color: '#FFFFFF',
                    align: 'right',
                    format: '{point.y}',
                    y: 10, // 10 pixels down from the top
                    style: {
                        fontSize: '13px',
                        fontFamily: 'Verdana, sans-serif'
                    }
                }
            }]
        });
    }).fail(function (jqxhr, textStatus, error) {
        alert("Request Failed: " + error);
    });
});