<script>
    import "carbon-components-svelte/css/white.css";
    import { Header, Content, Grid, Row, Column, Button } from "carbon-components-svelte";
    import { onMount } from "svelte";

    let port;
    let connected = false;
    let message = "";

    async function connectToArduino() {
        try {
            port = await navigator.serial.requestPort();
            await port.open({ baudRate: 9600 });
            connected = true;
            console.log('Connected to Arduino');

            readFromArduino();
        } catch (error) {
            console.error('Error connecting to Arduino:', error);
        }
    }

    async function readFromArduino() {
        const reader = port.readable.getReader();

        try {
            while (true) {
                const { value, done } = await reader.read();
                if (done) {
                    reader.releaseLock();
                    break;
                }
                message = new TextDecoder().decode(value);
                console.log('Received:', message);
            }
        } catch (error) {
            console.error('Error reading from Arduino:', error);
        }
    }

    async function sendToArduino(data) {
        if (!port) return;
        const writer = port.writable.getWriter();
        await writer.write(new TextEncoder().encode(data));
        writer.releaseLock();
    }

    onMount(() => {
        if (!navigator.serial) {
            console.error('Web Serial API not supported in this browser');
        }
    });
</script>

<Header company="Hollow Clock" platformName="Configurator" />

<Content>
    <Grid padding={true}>
        <Row>
            <Column>
                <h1>Welcome to the Hollow Clock Configurator</h1>
                <p>This is where you can customize your Hollow Clock settings.</p>
                
                <Button on:click={connectToArduino} disabled={connected}>
                    {connected ? 'Connected' : 'Connect to Arduino'}
                </Button>

                {#if connected}
                    <p>Last message received: {message}</p>
                    <Button on:click={() => sendToArduino('Hello Arduino!')}>
                        Send Test Message
                    </Button>
                {/if}
                
                <!-- Add more configuration options and components here -->
            </Column>
        </Row>
    </Grid>
</Content>