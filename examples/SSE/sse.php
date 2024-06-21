<?php

set_time_limit(600); // SSE time out in seconds.

header('Content-Type: text/event-stream');
header('Cache-Control: no-cache');

while (true) {

    $time = date('r');
    echo "data: The server time is: {$time}\n\n";

    if (ob_get_level() > 0) {
        ob_flush();
    }

    flush();

    if (connection_aborted()) {
        return;
    }

    sleep(2); // Event interval in seconds.
}

?>