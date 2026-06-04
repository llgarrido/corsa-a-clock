var Index = (function () {
    var clockElement;
    var statusMessageElement;

    function updateClock() {
        const datetime = new Date();
        const localDate = new Date(datetime.getTime() - datetime.getTimezoneOffset() * 60000);
        clockElement.textContent = localDate.toISOString().slice(0, 19).replace('T', ' ');
    }

    function setStatusMessage(message, state) {
        statusMessageElement.textContent = message;
        statusMessageElement.className = 'validation-result' + (state ? ' ' + state : '');
    }

    function synchronizeDateTime() {
        setStatusMessage('');

        const datetime = new Date();
        const url = '/set?yyyy=' + datetime.getFullYear() +
            '&MM=' + (datetime.getMonth() + 1) +
            '&dd=' + datetime.getDate() +
            '&HH=' + datetime.getHours() +
            '&mm=' + datetime.getMinutes() +
            '&ss=' + datetime.getSeconds();
        fetch(url)
            .then(async function (r) {
                const message = r.ok ? 'Date and time synchronized successfully' : await r.text() || 'Failed to synchronize date and time';
                const state = r.ok ? 'ok' : 'error';
                setStatusMessage(message, state);
            })
            .catch(function (error) {
                const message = 'Failed to synchronize date and time: ' + error.message;
                setStatusMessage(message, 'error');
            });
    }

    function initialize() {
        clockElement = document.getElementById('clock');
        statusMessageElement = document.getElementById('status-message');
        updateClock();
        setInterval(updateClock, 500);
    }

    return {
        initialize: initialize,
        synchronizeDateTime: synchronizeDateTime
    };
})();


window.onload = Index.initialize;
