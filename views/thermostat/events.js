<script src="../assets/fullcalendar/moment.min.js" type="text/javascript"></script>
<script src="../assets/fullcalendar/fullcalendar.min.js" type="text/javascript"></script>
<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>

<script>
var CompCalendar = function() {
    var calendarEvents  = $('.calendar-events');

    /* Function for initializing drag and drop event functionality */
    var initEvents = function() {
        $('#external-events .fc-event').each(function() {
            // create an Event Object (http://arshaw.com/fullcalendar/docs/event_data/Event_Object/)
            var eventObject = { title: $.trim($(this).text()), color: $(this).css('background-color') };
            
            eventObject.heater_id = $(this).data("id");
            eventObject.occurrenceNumber = $(this).data("occurrence");

            // store the Event Object in the DOM element so we can get to it later
            $(this).data('eventObject', eventObject);

            // make the event draggable using jQuery UI
            $(this).draggable({ zIndex: 999, revert: true, revertDuration: 0 });
        });
    };

    return {
        init: function() {
            /* Initialize drag and drop event functionality */
            initEvents();

            /* Add new event in the events list */
            var eventInput      = $('#add-event');
            var eventInputVal   = '';

            // When the add button is clicked
            $('#add-event-btn').on('click', function(){
                // Get input value
                eventInputVal = eventInput.prop('value');

                // Check if the user entered something
                if ( eventInputVal ) {
                    // Add it to the events list
                    calendarEvents.append('<li class="animation-slideDown">' + $('<div />').text(eventInputVal).html() + '</li>');

                    // Clear input field
                    eventInput.prop('value', '');

                    // Init Events
                    initEvents();
                }

                // Don't let the form submit
                return false;
            });

            /* Initialize FullCalendar */
            var date = new Date();
            var d = date.getDate();
            var m = date.getMonth();
            var y = date.getFullYear();

            $('#calendar').fullCalendar({
                header: {
                    left: 'prev,next',
                    center: 'title',
                    right: 'month,agendaWeek,agendaDay'
                },
                displayEventEnd: true,
                defaultView: 'agendaWeek',
                height: 700,
                firstDay: 1,
                scrollTime: '10:00:00',
                timeFormat: 'H(:mm)',
                editable: true,
                droppable: true,
                drop: function(date, allDay) { // this function is called when something is dropped

                    // retrieve the dropped element's stored Event Object
                    var originalEventObject = $(this).data('eventObject');

                    // we need to copy it, so that multiple events don't have a reference to the same object
                    var copiedEventObject = $.extend({}, originalEventObject);
                    
                    var endDate = moment(date);
                    endDate.add(1, 'hours');
                    
                    addEvent(copiedEventObject.heater_id, date, endDate, copiedEventObject.occurrenceNumber);
                },
                eventClick: function(event, jsEvent, view) {
                    $( "#FormEditEvent" ).data("event", event);
                    $( "#modalEventEditTitle" ).text(event.start.format('HH:mm') + " " + event.end.format('HH:mm'));
                    $( "#modalEventEdit" ).modal();
                },
                eventResize: function(event, delta, revertFunc) {
                    updateEvent(event.eventId, event.occurrenceId, event.start, event.end, event.isRecurrent, revertFunc);
                },
                eventDrop: function(event, delta, revertFunc) {
                    updateEvent(event.eventId, event.occurrenceId, event.start, event.end, event.isRecurrent, revertFunc);
                },
                events: function(start, end, timezone, callback) {                  
                    var param = {
                        start: start.format('YYYY-MM-DD'),
                        end: end.format('YYYY-MM-DD')
                    };
                    
                    $.getJSON('events.js', param).done(function(result) {
                        if (result.success) {
                            var events = [];
                            $.each(result.records, function(key, event) {
                                var object = {};
                                
                                if (event.recurrent_date) {
                                    object.id = event.event_id;
                                    object.isRecurrent = true;
                                } else {
                                    object.isRecurrent = false;
                                }
                                
                                object.eventId = event.event_id;
                                object.occurrenceId = event.occurrence_id;
                                object.title = ((event.recurrent_date)?"(R) ":"") + "Heat";
                                object.start = new Date(event.start_date);
                                object.end = new Date(event.end_date);
                                object.allDay = false;
                                object.color = (event.heater_id==1)?"#1abc9c":"#9b59b6";
                                
                                events.push(object);
                            });
                            callback(events);
                        } else {
                            alert_error(result.msg);
                        }
                    }).fail(function(jqxhr, textStatus, error) {
                        alert_error("Request Failed: " + error);
                    });
                    
                }
            });
        }
    };
}();

$(function(){ CompCalendar.init(); });

$('#FormEditEvent').on('submit', function(e) {
    e.preventDefault();

    var event = $( "#FormEditEvent" ).data("event");
    var remove = $('input[name=radioRemove]:checked', '#FormEditEvent').val();

    if (remove === "event" && event.occurrenceId != 0) {
        deleteEvent(event.occurrenceId, false);

        if (event.isRecurrent) {
            event._id = "oc" + event.occurrenceId;
            $('#calendar').fullCalendar('updateEvent', event);
        }

        $('#calendar').fullCalendar('removeEvents', event._id);
        $( "#modalEventEdit" ).modal('hide');
    }

    if (remove === "all_occurrences" && event.isRecurrent) {
        deleteEvent(event.id, true)
        $('#calendar').fullCalendar('removeEvents', event._id);
        $( "#modalEventEdit" ).modal('hide');
    }
});

function addEvent(heaterId, startDate, endDate, occurrence_number) {
    var param = {
        heater_id: heaterId,
        start_date: startDate.format('YYYY-MM-DD%20HH:mm:ss'),
        end_date: endDate.format('YYYY-MM-DD%20HH:mm:ss'),
        setpoint: 'heat',
        occurrence_number: occurrence_number
    };

    $.getJSON('add_event', param)
        .done(function(result) {
            if (result.success) {    
                $('#calendar').fullCalendar('refetchEvents');
        
                $.bootstrapGrowl('<h4>Success!</h4> <p>' + startDate.format('HH:mm') + ' to ' + endDate.format('HH:mm') + '</p>', {
                    type: "success",
                    delay: 2500,
                    allow_dismiss: true
                });
            } else {
                $.bootstrapGrowl('<h4>Error!</h4> <p>' + result.msg  + '</p>', {
                    type: "danger",
                    delay: 0,
                    allow_dismiss: true
                });
            }
        }).fail(function(jqxhr, textStatus, error) {
            $.bootstrapGrowl('<h4>Error!</h4> <p>Request Failed: ' + error  + '</p>', {
                type: "danger",
                delay: 0,
                allow_dismiss: true
            });
    });
};

function updateEvent(eventId, occurrenceId, startTime, endTime, allOccurrence, revertFunc) {
    var param = {
        event_id: eventId,
        occurrence_id: occurrenceId,
        start_date: startTime.format('YYYY-MM-DD%20HH:mm:ss'),
        end_date: endTime.format('YYYY-MM-DD%20HH:mm:ss'),
        all_occurrences: allOccurrence
    };

    $.getJSON('set_event_time', param)
        .done(function(result) {
            if (result.success) {
                $.bootstrapGrowl('<h4>Success!</h4> <p>' + startTime.format('HH:mm') + ' to ' + endTime.format('HH:mm') + '</p>', {
                    type: "success",
                    delay: 2500,
                    allow_dismiss: true
                });
            } else {
                $.bootstrapGrowl('<h4>Error!</h4> <p>' + result.msg  + '</p>', {
                    type: "danger",
                    delay: 0,
                    allow_dismiss: true
                });
                revertFunc();
            }
        }).fail(function(jqxhr, textStatus, error) {
            $.bootstrapGrowl('<h4>Error!</h4> <p>Request Failed: ' + error  + '</p>', {
                type: "danger",
                delay: 0,
                allow_dismiss: true
            });
            revertFunc();
    });
};

function deleteEvent(id, allOccurrence) {
    var param = {};
    
    if (allOccurrence) {
        param.event_id = id;
    } else {
        param.occurrence_id = id;
    }

    $.getJSON('delete_event', param)
        .done(function(result) {
            if (result.success) {
                $.bootstrapGrowl('<h4>Success!</h4> <p>Deleted with success</p>', {
                    type: "success",
                    delay: 2500,
                    allow_dismiss: true
                });
            } else {
                $.bootstrapGrowl('<h4>Error!</h4> <p>' + result.msg  + '</p>', {
                    type: "danger",
                    delay: 0,
                    allow_dismiss: true
                });
                revertFunc();
            }
        }).fail(function(jqxhr, textStatus, error) {
            $.bootstrapGrowl('<h4>Error!</h4> <p>Request Failed: ' + error  + '</p>', {
                type: "danger",
                delay: 0,
                allow_dismiss: true
            });
            revertFunc();
    });
};

</script>