// Add the nodes option through an event call. We want to start with the parent
// item and apply separate colors to each child element, then the same color to
// grandchildren.
Highcharts.addEvent(
  Highcharts.Series,
  'afterSetOptions',
  function (e) {
    var colors = Highcharts.getOptions().colors,
      i = 0,
      nodes = {};

    if (
      this instanceof Highcharts.seriesTypes.networkgraph &&
      e.options.id === 'lang-tree'
    ) {
      e.options.data.forEach(function (link) {

        if (link[0] === 'Gateway') {
          nodes['Gateway'] = {
            id: 'Gateway',
            marker: {
              radius: 20
            }
          };
          nodes[link[1]] = {
            id: link[1],
            marker: {
              radius: 10
            },
            color: colors[i++]
          };
        } else if (nodes[link[0]] && nodes[link[0]].color) {
          nodes[link[1]] = {
            id: link[1],
            color: colors[i++]
          };
        }
      });

      e.options.nodes = Object.keys(nodes).map(function (id) {
        return nodes[id];
      });
    }
  }
);

$.getJSON('/mysensors/routing.js', function (result) {
  var data = [];

  for (row of result.data) {
    data.push([rename(row.parent), rename(row.node)]);
  }

  Highcharts.chart('graph_container', {
    chart: {
      type: 'networkgraph',
      height: '550px'
    },
    title: {
      text: 'MySensors Routing'
    },
    plotOptions: {
      networkgraph: {
        keys: ['from', 'to'],
        layoutAlgorithm: {
          enableSimulation: true,
          friction: -0.9
        }
      }
    },
    series: [{
      dataLabels: {
        enabled: true,
        linkFormat: '',
        allowOverlap: true
      },
      id: 'lang-tree',
      data: data
    }]
  });
}).fail(function (jqxhr, textStatus, error) {
  alert("Request Failed: " + error);
});

function rename(name) {
  if (name == "0") {
    return 'Gateway';
  } else {
    return name;
  }
}