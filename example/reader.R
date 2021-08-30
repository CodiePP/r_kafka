# R Kafka interface
# example reader and shiny dashboard
# Copyright 2021 A. Diemand

library(dplyr)
library(ggplot2)
library(rkafka)
library(shiny)

# settings
KHOST <- "localhost"
KPORT <- "9092"
TOPIC <- "channel001"

# Define UI for application that draws a histogram
ui <- fluidPage(

    # Application title
    titlePanel("R Kafka example"),

    # Sidebar with a slider input for number of bins 
    sidebarLayout(
        sidebarPanel(
            sliderInput("updatefreq",
                        "Update frequency (ms):",
                        min = 100,
                        max = 10000,
                        value = 500)
        ),

        # Show a plot of the generated distribution
        mainPanel(
           plotOutput("myPlot")
        )
    )
)

# Define server logic required to draw a histogram
server <- function(input, output, session) {

    # setup Kafka reader
    kconf <- rkafka::kafka_conf_new()
    rkafka::kafka_conf_set(kconf, "client.id", "me-reader")
    rkafka::kafka_conf_set(kconf, "group.id", "1")
    rkafka::kafka_conf_set(kconf, "bootstrap.servers", paste0(KHOST,":",KPORT))
    
    kc <- rkafka::kafka_consumer_new(kconf)
    topics <- data.frame(topic = c(TOPIC))
    rkafka::kafka_subscribe(kc, topics)
    
    # the timeseries
    session$userData$ts <- data.frame(ts = c(), time = c())

    # first data point
    dataupdate <- reactiveVal(TRUE)

    observe({
        updatefreq <- max(min(10000,input$updatefreq),100)
        invalidateLater(updatefreq, session)
        res <- rkafka::kafka_consumer_poll(kc, 100)
        if (!is.null(res) && !is.null(res$value) && length(res$value) > 0) {
            now <- Sys.time()
            session$userData$ts <- rbind(session$userData$ts,
                                         c(ts = as.numeric(res$value), time = now)) %>%
                                   slice_tail(n = 30)
            colnames(session$userData$ts) <- c('ts','time')
            dataupdate(TRUE)  ## signal
        }
    })
    
    observe({
        req(dataupdate())
        if (nrow(session$userData$ts) > 1) {
            output$myPlot <- renderPlot({
                p <- ggplot(session$userData$ts, aes(x = time, y = ts)) +
                     geom_line(color = 'cyan', size = 3) +
                     geom_point(color = 'blue', size = 5) +
                     xlab(NULL)
                p
            })
        }
        isolate({ dataupdate(FALSE) })
    })
}

# Run the application 
shinyApp(ui = ui, server = server)
