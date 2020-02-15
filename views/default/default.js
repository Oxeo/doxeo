var updateError = false;
var socket = new WebSocket("ws://" + window.location.hostname + ":8081");

jQuery(document).ready(function () {
    update();
    updateScriptPanel();
    updateCameraPanel();
    setInterval(refreshCameras, 1000);
});

socket.onopen = function (event) {
    console.log("Websocket connected!");

    this.onclose = function (event) {
        console.log("Websocket closed!");
    };

    this.onmessage = function (event) {
        console.log("Message:", event.data);
        update();
    };
};

function update() {
    if (updateError) {
        return;
    }

    $.getJSON('system.js').done(function (result) {
        if (result.success) {
            $('#system_time').html(result.time);
            if (result.device_connected) {
                $('#board_status').html('<span class="label label-info">Connected</span>');
            } else {
                $('#board_status').html('<span class="label label-danger">Disconnected</span>');
            }
        } else {
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function (jqxhr, textStatus, error) {
        updateError = true;
        alert_error("Request Failed: " + error);
    });

    $.getJSON('switch/list.js').done(function (result) {
        if (result.Result == "OK") {
            $('#switchList').html('');
            result.Records.sort(function (a, b) {
                return a.order - b.order;
            })
            $.each(result.Records, function (key, val) {
                if (val.is_visible != "true") {
                    return true;
                }
                $('#switchList').append('<tr><td class="text-right" style="width: 50%">' + val.name + '</td><td class="text-left"><input id="sw_' + val.id + '" name="switch" class="switch_on_off" type="checkbox" data-id="' + val.id + '" data-size="mini"></td></tr>');
                if (val.status == "on" || val.status == "pending") {
                    $('#sw_' + val.id + '').prop('checked', true);
                }
            });
            $("[name='switch']").bootstrapSwitch();
        } else {
            $('#switchList').html('<tr><td></td></tr>');
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function (jqxhr, textStatus, error) {
        $('#switchList').html('<tr><td></td></tr>')
        updateError = true;
        alert_error("Request Failed: " + error);
    });

    $.getJSON('thermostat/heaters.js').done(function (result) {
        if (result.Result == "OK") {
            var create = ($('#heater_pan1').length == 0) ? false : true;

            $.each(result.Records, function (key, heater) {
                if (create) {
                    if (key != 0) {
                        $('#heater_nav_tabs li:last-child').clone().insertAfter('#heater_nav_tabs li:last-child');
                        $('#heater_nav_tabs li:last-child').removeClass("active");
                        $('#heater_tab_panes div.tab-pane:last-child').clone().insertAfter('#heater_tab_panes div.tab-pane:last-child');
                        $('#heater_tab_panes div.tab-pane:last-child').removeClass("active");
                    }

                    if (heater.id === 1) {
                        $('#heater_nav_tabs li:last-child').addClass("active");
                        $('#heater_tab_panes div.tab-pane:last-child').addClass("active");
                    }

                    $('#heater_nav_tabs li:last-child').find("a").text(heater.name);
                    $('#heater_nav_tabs li:last-child').find("a").prop('href', '#heater_' + heater.id);
                    $('#heater_nav_tabs li:last-child').find("a").attr('aria-controls', 'heater_' + heater.id);
                    $('#heater_tab_panes div.tab-pane:last-child').attr('id', 'heater_' + heater.id);
                }

                $('#heater_' + heater.id).find(".heater_temperature").html(heater.temperature.toFixed(2) + '&deg;');
                $('#heater_' + heater.id).find(".heat_setpoint").text(heater.heat_setpoint.toFixed(1));
                $('#heater_' + heater.id).find(".cool_setpoint").text(heater.cool_setpoint.toFixed(1));
                $('#heater_' + heater.id).find(".heater_mode").text(heater.mode);
                $('#heater_' + heater.id).find(".heater_status").text(heater.status);

                if (heater.current_setpoint == heater.heat_setpoint) {
                    $('#heater_' + heater.id).find(".heat_setpoint").css("font-weight", "Bold");
                } else {
                    $('#heater_' + heater.id).find(".heat_setpoint").css("font-weight", "Normal");
                }

                if (heater.current_setpoint == heater.cool_setpoint) {
                    $('#heater_' + heater.id).find(".cool_setpoint").css("font-weight", "Bold");
                } else {
                    $('#heater_' + heater.id).find(".cool_setpoint").css("font-weight", "Normal");
                }
            });
        } else {
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function (jqxhr, textStatus, error) {
        updateError = true;
        alert_error("Request Failed: " + error);
    });

    $.getJSON('thermostat/status.js').done(function (result) {
        if (result.success) {
            if (result.thermostat_status === 1) {
                $('#thermostat_status_active').find('span').first().text("Running");
                $('#thermostat_status_active').removeClass('btn-secondary btn-info btn-warning').addClass('btn-info');
            } else if (result.thermostat_status === 2) {
                $('#thermostat_status_active').find('span').first().text("On Break");
                $('#thermostat_status_active').removeClass('btn-secondary btn-info btn-warning').addClass('btn-warning');
            } else {
                $('#thermostat_status_active').find('span').first().text("Stopped");
                $('#thermostat_status_active').removeClass('btn-secondary btn-info btn-warning').addClass('btn-secondary');
            }

            if (result.temperaturelogger_enable) {
                $('#temperaturelogger_status_active').find('span').first().text("Running");
                $('#temperaturelogger_status_active').removeClass('btn-secondary btn-info').addClass('btn-info');
            } else {
                $('#temperaturelogger_status_active').find('span').first().text("Stopped");
                $('#temperaturelogger_status_active').removeClass('btn-secondary btn-info').addClass('btn-secondary');
            }
        } else {
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function (jqxhr, textStatus, error) {
        updateError = true;
        alert_error("Request Failed: " + error);
    });

    $.getJSON('logs.js?type=warning').done(function (result) {
        if (result.success) {
            var cptWarning = 0;
            var cptCritical = 0;

            $('#errorModalTable').html("");
            $('#warningModalTable').html("");

            $.each(result.messages, function (key, val) {
                if (val.type === "critical") {
                    cptCritical++;
                    $('#errorModalTable').append('<tr><td>' + val.date + '</td><td>' + val.message + '</td></tr>')
                } else if (val.type === "warning") {
                    cptWarning++;
                    $('#warningModalTable').append('<tr><td>' + val.date + '</td><td>' + val.message + '</td></tr>')
                }
            });

            if (cptCritical > 0) {
                $('#critical_error').html('<button type="button" class="btn btn-danger btn-xs" data-toggle="modal" data-target="#errorModal">' + cptCritical + '</button>');
            } else {
                $('#critical_error').html('<span class="label label-success">0</span>');
            }

            if (cptWarning > 0) {
                $('#warning_error').html('<button type="button" class="btn btn-danger btn-xs" data-toggle="modal" data-target="#warningModal">' + cptWarning + '</button>');
            } else {
                $('#warning_error').html('<span class="label label-success">0</span>');
            }
        } else {
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function (jqxhr, textStatus, error) {
        updateError = true;
        alert_error("Request Failed: " + error);
    });

    $.getJSON('sensor/list.js').done(function (result) {
        if (result.Result == "OK") {
            $('#sensor_tab_panes .table').empty();
            $('#batteryList').html('');
            var batteryNames = [];
            result.Records.sort(function (a, b) {
                if (a.order < b.order) {
                    return -1;
                } else if (a.order > b.order) {
                    return 1;
                } else {
                    return 0;
                }
            })
            $.each(result.Records, function (key, val) {
                // update sensor panel
                if (val.visibility != "hide") {
                    var date = new Date(val.last_event * 1000);;
                    var lastUpdate = date.toLocaleTimeString() + ' ' + date.toLocaleDateString();
                    var target = '#sensor_pan_automatisme';

                    if (val.category === 'door' || val.category === 'window' || val.category === 'doormat' || val.category === 'pir' || val.category === 'doorknob') {
                        target = '#sensor_pan_security';
                    } else if (val.category === 'temperature') {
                        target = '#sensor_pan_energy'
                    } else if (val.category === 'heartbeat') {
                        target = '#sensor_pan_mon'
                    }
                    $(target + ' .table').append('<tr><td class="text-right" style="width: 40%"><span data-toggle="tooltip" data-placement="left" title="' + lastUpdate + '">' + getSensorImage(val) + '</span></td><td class="text-left">' + val.name + '<br />' + getSensorStatus(val) + '</td></tr>');
                }

                // update battery panel
                if (val.battery > 0 && !batteryNames.includes(val.full_name)) {
                    batteryNames.push(val.full_name);
                    date = new Date(val.battery_update * 1000);
                    lastUpdate = date.toLocaleTimeString() + ' ' + date.toLocaleDateString();
                    $('#batteryList').append('<tr><td class="text-right" style="width: 50%">' + val.full_name + '</td><td class="text-left"><span data-toggle="tooltip" data-placement="right" title="' + lastUpdate + '">' + val.battery + '%</span></td></tr>');
                }
            });
            $('[data-toggle="tooltip"]').tooltip();
        } else {
            $('#sensorList').html('<tr><td></td></tr>');
            $('#batteryList').html('<tr><td></td></tr>');
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function (jqxhr, textStatus, error) {
        $('#sensorList').html('<tr><td></td></tr>');
        $('#batteryList').html('<tr><td></td></tr>');
        updateError = true;
        alert_error("Request Failed: " + error);
    });

    $.getJSON('scenario/list.js').done(function (result) {
        if (result.Result == "OK") {
            $('#scenarioList').html('');
            result.Records.sort(function (a, b) {
                if (a.order < b.order) {
                    return -1;
                } else if (a.order > b.order) {
                    return 1;
                } else {
                    return 0;
                }
            })
            $.each(result.Records, function (key, val) {
                if (val.hide != "true") {
                    $('#scenarioList').append('<tr><td class="text-right" style="width: 50%">' + val.name + '</td><td class="text-left"></td></tr>');
                }
            });
        } else {
            $('#scenarioList').html('<tr><td></td></tr>');
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function (jqxhr, textStatus, error) {
        $('#scenarioList').html('<tr><td></td></tr>');
        updateError = true;
        alert_error("Request Failed: " + error);
    });
}

function updateCameraPanel() {
    $.getJSON('camera/list.js').done(function (result) {
        if (result.Result == "OK") {
            $('#cameraList').html('');
            $.each(result.Records, function (key, val) {
                $('#cameraList').append('<tr><td class="camera"><a href="/camera/image/?id=' + val.id + '"><img src="/camera/image/?id=' + val.id + '&u=1" style="max-width:100%; min-width:50px; min-height:50px"></a></td></tr>');
            });
        } else {
            $('#cameraList').html('<tr><td></td></tr>');
            alert_error(result.msg);
        }
    }).fail(function (jqxhr, textStatus, error) {
        $('#cameraList').html('<tr><td></td></tr>');
        alert_error("Request Failed: " + error);
    });
}

function refreshCameras() {
    $('.camera img').attr('src', function (i, old) { return old.replace(/&u.+/, "&u=" + (Math.random() * 1000)); });
}

function updateScriptPanel() {
    $.getJSON('script/list.js').done(function (result) {
        $('#scriptPanel .content').empty();
        if (result.Result == "OK") {
            result.Records.sort(function (a, b) {
                if (a.name < b.name) {
                    return -1;
                } else if (a.name > b.name) {
                    return 1;
                } else {
                    return 0;
                }
            })
            var number = 0;
            const numberByPage = 10;
            $.each(result.Records, function (key, val) {

                if (val.visibility === 'hide') {
                    return;
                }

                if ((number % numberByPage) == 0) {
                    var pageNumber = number / numberByPage + 1;
                    $('#scriptPanel .content').append('<table class="table" id="scriptPanel-page-' + pageNumber + '" style="margin:0px; border-bottom: 1px solid #ddd"><tbody></tbody></table>');
                    $('#scriptPanel .pagination').append('<li><a href="#">' + pageNumber + '</a></li>');
                }

                status = "\
				<div class=\"dropdown\"> \
				  <button class=\"btn btn-xs dropdown-toggle\" type=\"button\" style=\"border: 0px; padding:0px 5px 1px 5px; font-size:11px;\" id=\"script_status_"+ val.id + "\" data-toggle=\"dropdown\" aria-haspopup=\"true\" aria-expanded=\"false\"> \
					<span></span> <span class=\"caret\"></span> \
				  </button> \
				  <ul class=\"dropdown-menu\" aria-labelledby=\"dropdownMenuMode\"> \
					<li><a href='javascript:void(0)' class='change_script_status' data-id='"+ val.id + "' data-status='on'>Enable</a></li> \
					<li><a href='javascript:void(0)' class='change_script_status' data-id='"+ val.id + "' data-status='off'>Disable</a></li> \
				  </ul> \
				</div>";

                $('#scriptPanel .content').find('tbody:last').append('<tr><td class="text-right" style="width: 50%">' + val.name + '</td><td class="text-left">' + status + '</td></tr>');

                if (val.status === "on") {
                    $('#script_status_' + val.id).find('span').first().text("Enabled");
                    $('#script_status_' + val.id).removeClass('btn-secondary btn-info').addClass('btn-info');
                } else {
                    $('#script_status_' + val.id).find('span').first().text("Disabled");
                    $('#script_status_' + val.id).removeClass('btn-secondary btn-info').addClass('btn-secondary');
                }

                number++;
            });

            managePagination('#scriptPanel');
        } else {
            alert_error(result.msg);
        }
    }).fail(function (jqxhr, textStatus, error) {
        alert_error("Request Failed: " + error);
    });
}

$('#switchList').on('switchChange.bootstrapSwitch', '.switch_on_off', function (event, state) {
    var data = $(this).data();

    param = {
        id: data.id,
        status: (state == true) ? "on" : "off"
    };

    $.getJSON('switch/change_switch_status', param)
        .done(function (result) {
            if (result.success == false) {
                alert_error(result.msg);
            }
        }).fail(function (jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
        });
});

$(".clear_logs").click(function () {
    var data = $(this).data();

    param = {
        id: 0,
        type: data.clear
    };

    $.getJSON('clear_logs.js', param)
        .done(function (result) {
            if (result.success) {
                update();
            } else {
                alert_error(result.msg);
            }
        }).fail(function (jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
        });
});

$(".stop_application").click(function () {
    $.getJSON('stop')
        .done(function (result) {
            if (result.success) {
                updateError = true;
                alert_info("Application stopped.");
            } else {
                alert_error(result.msg);
            }
        }).fail(function (jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
        });
});

$(".logout").click(function () {
    $.getJSON('auth/js_logout')
        .done(function (result) {
            if (result.success) {
                window.location.replace("/auth");
            } else {
                alert_error(result.msg);
            }
        }).fail(function (jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
        });
});

$('.container').on('click', '.change_heater_mode', function () {
    var data = $(this).data();
    var id = $(this).parentsUntil(".tab-pane").parent().attr('id').match(/\d+/g)[0];

    param = {
        heater: id,
        mode: data.mode
    };

    $.getJSON('thermostat/set_mode', param)
        .done(function (result) {
            if (result.success) {
                update();
            } else {
                alert_error(result.msg);
            }
        }).fail(function (jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
        });
});

$('.container').on('click', '.change_thermostat_status', function () {
    var data = $(this).data();

    $.getJSON('thermostat/set_status', data)
        .done(function (result) {
            if (result.success) {
                update();
            } else {
                alert_error(result.msg);
            }
        }).fail(function (jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
        });
});

$('.container').on('click', '.change_script_status', function () {
    var data = $(this).data();
    var text = $(this).text();

    $('#script_status_' + data.id).find('span').first().text("...");

    $.getJSON('script/set_status', data)
        .done(function (result) {
            if (result.success) {
                if (result.status === "on") {
                    $('#script_status_' + data.id).find('span').first().text("Enabled");
                    $('#script_status_' + data.id).removeClass('btn-secondary btn-info').addClass('btn-info');
                } else {
                    $('#script_status_' + data.id).find('span').first().text("Disabled");
                    $('#script_status_' + data.id).removeClass('btn-secondary btn-info').addClass('btn-secondary');
                }
            } else {
                alert_error(result.msg);
            }
        }).fail(function (jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
        });
});

$('.container').on('click', '.change_heater_setpoint', function () {
    var data = $(this).data();
    var id = $(this).parentsUntil(".tab-pane").parent().attr('id').match(/\d+/g)[0];

    if (data.type === "heat") {
        temperature = parseFloat($('#heater_' + id).find(".heat_setpoint").text());
    } else {
        temperature = parseFloat($('#heater_' + id).find(".cool_setpoint").text());
    }

    if (isNaN(temperature)) {
        temperature = 0;
    }

    if (data.increment) {
        temperature += 0.5;
    } else {
        temperature -= 0.5;
    }

    param = {
        heater: id
    };

    if (data.type === "heat") {
        param.heat_temperature = temperature;
    } else {
        param.cool_temperature = temperature;
    }

    $.getJSON('thermostat/set_setpoint', param)
        .done(function (result) {
            if (result.success) {
                update();
            } else {
                alert_error(result.msg);
            }
        }).fail(function (jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
        });
});

function getSensorImage(sensor) {
    var img = "";
    if (sensor.category === "temperature") {
        if (sensor.value == '' || sensor.value <= 15) {
            img = "temperature_blue.png";
        } else if (sensor.value <= 22) {
            img = "temperature_orange.png";
        } else {
            img = "temperature_red.png";
        }
    } else if (sensor.category === "humidity") {
        img = "humidity.png";
    } else if (sensor.category === "door") {
        if (isSensorOn(sensor)) {
            img = "door_close.png";
        } else {
            img = "door_open.png";
        }
    } else if (sensor.category === "window") {
        if (isSensorOn(sensor)) {
            img = "window_close.png";
        } else {
            img = "window_open.png";
        }
    } else if (sensor.category === "doorknob") {
        var eventDate = new Date(sensor.last_event * 1000);
        var todayDate = new Date();
        if (todayDate.getTime() - eventDate.getTime() < 180000) {
            img = "handle_on.png";
        } else {
            img = "handle_off.png";
        }
    } else if (sensor.category === "pir") {
        var eventDate = new Date(sensor.last_event * 1000);
        var todayDate = new Date();
        if (todayDate.getTime() - eventDate.getTime() < 180000) {
            img = "running.png";
        } else {
            img = "eye.png";
        }
    } else if (sensor.category === "pump") {
        img = "tap.png";
    } else if (sensor.category === "plant") {
        img = "pot.png";
    } else if (sensor.category === "speaker") {
        img = "music.png";
    } else if (sensor.category === "light") {
        img = "bulb.png";
    } else if (sensor.category === "heartbeat") {
        img = "signal.png";
    } else if (sensor.category === "doormat") {
        var eventDate = new Date(sensor.last_event * 1000);
        var todayDate = new Date();
        if (todayDate.getTime() - eventDate.getTime() < 180000) {
            img = "sensor.png";
        } else {
            img = "sensor_off.png";
        }
    } else {
        img = "sensor.png";
    }

    var style = '';

    if (sensor.value === '') {
        style = ' style="opacity: 0.2; filter: alpha(opacity=20)"'
    } else {
        style = isSensorOutdated(sensor) ? ' style="background-color:red"' : '';
    }

    return '<img src="assets/images/' + img + '" alt="Icon" height="45" width="45"' + style + '>';
}

function getSensorStatus(sensor) {
    var result = sensor.value;
    var date = new Date(sensor.last_event * 1000);

    if (result === '') {
        // nothing to do
    } else if (sensor.category === 'door' || sensor.category === 'window') {
        if (isSensorOn(sensor)) {
            result = "Closed " + timeAgo(date) + ' ago';
        } else {
            result = "Opened " + timeAgo(date) + ' ago';
        }
    } else if (sensor.category === 'temperature') {
        result = result + "°C (" + timeAgo(date) + " ago)";
    } else if (sensor.category === 'plant') {
        result = result + "% (" + timeAgo(date) + " ago)";
    } else if (sensor.category === 'humidity') {
        result = result + "% (" + timeAgo(date) + " ago)";
    } else if (sensor.category === 'light') {
        result = result + "% (" + timeAgo(date) + " ago)";
    } else if (sensor.category === 'doorknob') {
        result = timeAgo(date) + ' ago';
    } else if (sensor.category === 'pir') {
        result = timeAgo(date) + ' ago';
    } else if (sensor.category === 'doormat') {
        result = timeAgo(date) + ' ago';
    } else if (sensor.category === 'heartbeat') {
        result = timeAgo(date);
    } else {
        result = result + " (" + timeAgo(date) + ' ago)';
    }

    return result;
}

function isSensorOn(sensor) {
    if (sensor.value === "") {
        return false;
    } else if (sensor.value === "on" || sensor.value === "1" || sensor.value === "close") {
        return sensor.invert_binary === 'false';
    } else {
        return sensor.invert_binary === 'true';
    }
}

function isSensorOutdated(sensor) {
    var eventDate = new Date(sensor.last_event * 1000);
    var todayDate = new Date();

    if (sensor.category === 'temperature' || sensor.category === 'humidity' || sensor.category === 'light') {
        if (todayDate.getTime() - eventDate.getTime() > 1200000) { // 20 minutes
            return true;
        }
    } else if (sensor.category === 'plant') {
        if (todayDate.getTime() - eventDate.getTime() > 21600000) { // 6 hours
            return true;
        }
    } else if (sensor.category === 'heartbeat') {
        if (todayDate.getTime() - eventDate.getTime() > 60000) { // 60 seconds
            return true;
        }
    }

    return false;
}

function alert_info(message) {
    $('#alert_placeholder').prepend(
        '<div style="display: none;" class="alert alert-info fade in" role="alert">' +
        '<strong>Info!</strong> ' + message +
        '<button type="button" class="close" data-dismiss="alert" aria-label="Close">' +
        '<span aria-hidden="true">&times;</span>' +
        '</button>' +
        '</div>'
    );

    $('.alert-info').slideDown("fast");
}

function alert_success(message) {
    $('#alert_placeholder').prepend(
        '<div style="display: none;" class="alert alert-success fade in" role="alert">' +
        '<strong>Succes!</strong> ' + message +
        '<button type="button" class="close" data-dismiss="alert" aria-label="Close">' +
        '<span aria-hidden="true">&times;</span>' +
        '</button>' +
        '</div>'
    );

    $('.alert-success').slideDown("fast");
}

function alert_error(message) {
    $('#alert_placeholder').prepend(
        '<div style="display: none;" class="alert alert-danger fade in" role="alert">' +
        '<strong>Error!</strong> ' + message +
        '<button type="button" class="close" data-dismiss="alert" aria-label="Close">' +
        '<span aria-hidden="true">&times;</span>' +
        '</button>' +
        '</div>'
    );

    $('.alert-danger').slideDown("fast");
}

function managePagination(panelId) {
    $(panelId + ' .content table').hide();
    $(panelId + ' .content').find('table:first').show();
    $(panelId + '-pagination').twbsPagination({
        totalPages: $(panelId + ' .content table').length,
        visiblePages: 7,
        hideOnlyOnePage: true,
        first: '',
        last: '',
        prev: '',
        next: '',
        onPageClick: function (event, page) {
            $(panelId + ' .content table').hide();
            $(panelId + '-page-' + page).show();
        }
    });
}

function timeAgo(date) {
    var seconds = Math.floor((new Date() - date) / 1000);
    var interval = Math.floor(seconds / 31536000);

    if (interval > 1) {
        return interval + " years";
    }
    interval = Math.floor(seconds / 2592000);
    if (interval > 1) {
        return interval + " months";
    }
    interval = Math.floor(seconds / 86400);
    if (interval > 1) {
        return interval + " days";
    }
    interval = Math.floor(seconds / 3600);
    if (interval > 1) {
        return interval + " hours";
    }
    interval = Math.floor(seconds / 60);
    if (interval > 1) {
        return interval + " minutes";
    }
    return Math.floor(seconds) + " seconds";
}
