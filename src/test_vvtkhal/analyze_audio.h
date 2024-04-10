#define RTS_AUDIO_HEADER_LENGTH_MAX	0x84
#define COMPOSE_ID(a, b, c, d)	((a) | ((b)<<8) | ((c)<<16) | ((d)<<24))
#define WAV_RIFF		COMPOSE_ID('R', 'I', 'F', 'F')
#define WAV_WAVE		COMPOSE_ID('W', 'A', 'V', 'E')
#define WAV_FMT			COMPOSE_ID('f', 'm', 't', ' ')
#define WAV_DATA		COMPOSE_ID('d', 'a', 't', 'a')
#define WAV_FACT		COMPOSE_ID('f', 'a', 'c', 't')
#define WAV_LIST		COMPOSE_ID('L', 'I', 'S', 'T')

int samplerate_to_config(int sample_rate)
{
	int ret = -1;
	switch(sample_rate)
	{
	case 8000:
		ret = VVTK_AUDIO_SAMPLE_RATE_8_KHZ;
		break;
	case 16000:
		ret = VVTK_AUDIO_SAMPLE_RATE_16_KHZ;
		break;
	case 32000:
		ret = VVTK_AUDIO_SAMPLE_RATE_32_KHZ;
		break;
	case 44100:
		ret = VVTK_AUDIO_SAMPLE_RATE_44_1_KHZ;
		break;
	case 48000:
		ret = VVTK_AUDIO_SAMPLE_RATE_48_KHZ;
		break;
	default:
		break;
	}

	return ret;
}

int samplerate_from_config(int sample_rate)
{
	int ret = -1;
	switch(sample_rate)
	{
	case VVTK_AUDIO_SAMPLE_RATE_8_KHZ:
		ret = 8000;
		break;
	case VVTK_AUDIO_SAMPLE_RATE_16_KHZ:
		ret = 16000;
		break;
	case VVTK_AUDIO_SAMPLE_RATE_32_KHZ:
		ret = 32000;
		break;
	case VVTK_AUDIO_SAMPLE_RATE_44_1_KHZ:
		ret = 44100;
		break;
	case VVTK_AUDIO_SAMPLE_RATE_48_KHZ:
		ret = 48000;
		break;
	default:
		break;
	}

	return ret;
}

int analyze_audio_header(FILE *fp, vvtk_audio_config_t *config,
			 int *head_length, int *pcmdata_size)
{
	int ret;
	char buffer[RTS_AUDIO_HEADER_LENGTH_MAX];
	unsigned int size = 0;
	unsigned int id = 0;
	unsigned int riff_size = 0;
	unsigned int fmt_size = 0;
	unsigned int fact_size = 0;
	unsigned int list_size = 0;
	unsigned int data_size = 0;
	unsigned int sample_rate = 0;
	unsigned int audio_format = 0;

	ret = fread(buffer, 1, RTS_AUDIO_HEADER_LENGTH_MAX, fp);
	if (ret != RTS_AUDIO_HEADER_LENGTH_MAX) {
		printf("fail to read file header : %d\n", ret);
		return -1;
	}

	audio_format = (unsigned int)(buffer[0x14] & 0xff) |
		       ((unsigned int)(buffer[0x15] & 0xff) << 8);

	if(audio_format == 1) {
		config->codec = VVTK_AUDIO_CODEC_PCM;
	} else {
		return -1;
	}

	config->channel = (unsigned int)(buffer[0x16] & 0xff) |
			 ((unsigned int)(buffer[0x17] & 0xff) << 8);
	sample_rate = (unsigned int)(buffer[0x18] & 0xff) |
		     ((unsigned int)(buffer[0x19] & 0xff) << 8) |
		     ((unsigned int)(buffer[0x1a] & 0xff) << 16) |
		     ((unsigned int)(buffer[0x1b] & 0xff) << 24);

	if(samplerate_to_config(sample_rate) < 0)
		return -1;
	config->rate = samplerate_to_config(sample_rate);

	config->format = (unsigned int)(buffer[0x22] & 0xff) |
		       ((unsigned int)(buffer[0x23] & 0xff) << 8);
	riff_size = (unsigned int)(buffer[0x4] & 0xff) |
			((unsigned int)(buffer[0x5] & 0xff) << 8) |
			((unsigned int)(buffer[0x6] & 0xff) << 16) |
			((unsigned int)(buffer[0x7] & 0xff) << 24);
	fmt_size = (unsigned int)(buffer[0x10] & 0xff) |
			((unsigned int)(buffer[0x11] & 0xff) << 8) |
			((unsigned int)(buffer[0x12] & 0xff) << 16) |
			((unsigned int)(buffer[0x13] & 0xff) << 24);
	size = 0x14 + fmt_size;
	if (size + 3 >= RTS_AUDIO_HEADER_LENGTH_MAX) {
		printf("Error wav file\n");
		return -1;
	}
	id = (unsigned int)(buffer[size] & 0xff) |
			((unsigned int)(buffer[size + 1] & 0xff) << 8) |
			((unsigned int)(buffer[size + 2] & 0xff) << 16) |
			((unsigned int)(buffer[size + 3] & 0xff) << 24);

	if (id == WAV_LIST) {
		list_size = (unsigned int)(buffer[size + 4] & 0xff) |
			((unsigned int)(buffer[size + 5] & 0xff) << 8) |
			((unsigned int)(buffer[size + 6] & 0xff) << 16) |
			((unsigned int)(buffer[size + 7] & 0xff) << 24);
		size = size + 8 + list_size;

		id = (unsigned int)(buffer[size] & 0xff) |
			((unsigned int)(buffer[size + 1] & 0xff) << 8) |
			((unsigned int)(buffer[size + 2] & 0xff) << 16) |
			((unsigned int)(buffer[size + 3] & 0xff) << 24);
	}
	if (id == WAV_FACT) {
		if (size + 7 >= RTS_AUDIO_HEADER_LENGTH_MAX) {
			printf("Error wav file\n");
			return -1;
		}
		fact_size = (unsigned int)(buffer[size + 4] & 0xff) |
			((unsigned int)(buffer[size + 5] & 0xff) << 8) |
			((unsigned int)(buffer[size + 6] & 0xff) << 16) |
			((unsigned int)(buffer[size + 7] & 0xff) << 24);
		size = size + 8 + fact_size + 4;
	} else if (id == WAV_DATA)
		size = size + 4;
	else {
		printf("analysis header failed!\n");
		return -1;
	}

	if (size + 3 >= RTS_AUDIO_HEADER_LENGTH_MAX) {
		printf("Error wav file\n");
		return -1;
	}
	data_size = (unsigned int)(buffer[size] & 0xff) |
			((unsigned int)(buffer[size + 1] & 0xff) << 8) |
			((unsigned int)(buffer[size + 2] & 0xff) << 16) |
			((unsigned int)(buffer[size + 3] & 0xff) << 24);
	*head_length = size + 4;
	*pcmdata_size = data_size;

	return 0;
}