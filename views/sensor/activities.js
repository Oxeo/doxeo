$(function () {

    Highcharts.setOptions({
        global: {
            useUTC: false
        }
    });

    $.getJSON('sensor/msg_activities.js', function (result) {
        var sData = [];
        
        // Parse data
        $.each(result.records, function(key, val) {
            sData.push([val.date, val.number]);
        });
    
        // Create the chart
        $('#graph_container').highcharts('column', {
            xAxis: {
                type: 'category',
                labels: {
                    rotation: -45,
                    style: {
                        fontSize: '13px',
                        fontFamily: 'Verdana, sans-serif'
                    }
                }
            },
            yAxis: {
                min: 0,
                title: {
                    text: 'Population (millions)'
                }
            },
            legend: {
                enabled: false
            },
            tooltip: {
                pointFormat: 'Population in 2017: <b>{point.y:.1f} millions</b>'
            },

            series : [{
                name: 'Messages',
                data: sData,
                dataLabels: {
                    enabled: true,
                    rotation: -90,
                    color: '#FFFFFF',
                    align: 'right',
                    format: '{point.y:.1f}', // one decimal
                    y: 10, // 10 pixels down from the top
                    style: {
                        fontSize: '13px',
                        fontFamily: 'Verdana, sans-serif'
                    }
                }
            }]
        });
    }).fail(function(jqxhr, textStatus, error) {
        alert("Request Failed: " + error);
    });

});